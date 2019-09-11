/**
 * \file
 * \copyright  Copyright 2015-2016 PLUX - Wireless Biosignals, S.A.
 * \version    1.3
 * \date       March 2016
 */

#ifndef _MEMORYPLUXHEADER_
#define _MEMORYPLUXHEADER_

namespace Plux
{
   /** %Session schedule class.
   A session schedule contains all the information needed to start an internal acquisition session.
   \see MemoryDev::addSchedule(), MemoryDev::getSchedules()
   */
   struct Schedule
   {
      /** Scheduled session start time or condition (1 or a time_t value).
      If this attribute has the value 1, the scheduled session will start when the digital port input changes.
      Otherwise, the scheduled session will start when the device real-time clock
      reaches this value.
      
      This attribute value is unique to each schedule on the device,
      so this attribute is used as a schedule identifier in MemoryDev::deleteSchedule() and Session::schedStartTime.
      Default value is 1.
      */
      time_t   startTime;

      /** Scheduled session duration limit in seconds.
      If this attribute is zero, no session duration limit is set.
      An internal acquisition session can always be stopped by calling MemoryDev::stopSessionAcq().
      Default value is zero.
      */
      int      duration;
      int      nRepeats;      ///< Number of session repetitions (reserved, must be zero). Default value is zero.
      int      repeatPeriod;  ///< Repetition period in seconds (reserved, must be zero). Default value is zero.
      float    baseFreq;      ///< Acquisition base sampling frequency in Hertz. Default value is 1000.
      Sources  sources;       ///< Scheduled session sources.
      String   text;          ///< Optional user text to store in Session::text attribute of saved sessions (maximum of 70 characters).

      /** Initializes all structure fields.
      All arguments are optional. A structure field is initialized with the corresponding argument value, if provided.
      Otherwise, the field is initialized with its default value.
      */
      Schedule(time_t _startTime=1, int _duration=0, float _baseFreq=1000, Sources _sources=Sources(), String _text="") :
         startTime(_startTime), duration(_duration), baseFreq(_baseFreq), sources(_sources), text(_text), nRepeats(0),repeatPeriod(0) {}
   };
   typedef std::vector<Schedule> Schedules;  ///< Vector of Schedule type.

   /** Acquisition source class with properties attribute.
   The Session::sources attribute has this class to carry normal Source attributes and additional source properties.
   */
   struct SourceEx : public Source
   {
      Properties  properties; ///< Additional source properties.
   };
   typedef std::vector<SourceEx> SourcesEx;  ///< Vector of SourceEx type.

   /** Stored session header class.
   MemoryDev::getSessions() returns objects of this class.
   */
   struct Session
   {
      /** %Session start time.
      This attribute value is unique to each session stored on the device,
      so this attribute is used as a session identifier in MemoryDev::replaySession().
      */
      time_t      startTime;

      /** The Schedule::startTime attribute value of the schedule associated with this session,
      which is an unique schedule identifier.
      */
      time_t      schedStartTime;
      int         nFrames;       ///< Total number of frames in the stored session.
      float       baseFreq;      ///< %Session acquisition base sampling frequency in Hertz.
      SourcesEx   sources;       ///< %Session sources.
      String      text;          ///< Optional user text from the Schedule::text attribute of the schedule associated with this session.
      Properties  properties;    ///< Additional session properties.
   };
   typedef std::vector<Session> Sessions;  ///< Vector of Session type.

   /// Base class for PLUX signal-acquiring devices with internal memory.
   class MemoryDev : public SignalsDev
   {
   public:
      /** Connects to a PLUX device.
      \param path Path to the PLUX device. It has the same meaning as in BaseDev::BaseDev().
      */
      MemoryDev(const String &path);

      /** Promotes a BaseDev instance to MemoryDev.
      \param baseDev Instance to be promoted. If promotion is successful, \c baseDev is no longer valid.
      */
      MemoryDev(BaseDev &baseDev);

      /** Sets the device real-time clock.
      \param t Time to set on the device (optional). If this parameter is not given, current host time is used.
      */
      void   setTime(time_t t=0);

      /// Returns the device current real-time clock.
      time_t getTime(void);

      /** Returns all session schedules stored on the device.
      The returned schedules can refer to a running session (a running schedule, at most one such schedule) or
      to a session to run in the future. The schedules were previously added by calling addSchedule() or start().
      \param schs Reference to a #Schedules object to be filled by this method.
      \return Index to the running schedule in the returned vector, starting at 1; if zero, no schedule is running.
      \remarks On current devices firmware, only one schedule can be stored.
      */
      int  getSchedules(Schedules &schs);

      /** Adds a session schedule to the device.
      An internal acquisition session (i.e., an acquisition to internal memory) will start
      when the schedule start time or condition is met.
      \param sch %Schedule to add.
      \remarks On current devices firmware, only one schedule can be stored.
      */
      void addSchedule(const Schedule &sch);

      /** Deletes a session schedule from the device.
      \param startTime Schedule::startTime attribute value of the schedule to delete.
      \remarks A running schedule cannot be deleted.
      */
      void deleteSchedule(time_t startTime);

      /** Deletes all session schedules from the device.
      \remarks A running schedule cannot be deleted, so this method cannot be called while the device is acquiring.
      */
      void deleteAllSchedules(void);

      /** Stops an internal acquisition session.
      \remarks Call stop() to stop a real-time acquisition.
      */
      void stopSessionAcq(void);

      /** Returns the headers of all sessions stored on the device.
      \param sessions Reference to a #Sessions object to be filled by this method.
      \remarks This method cannot be called while the device is acquiring.
      */
      void getSessions(Sessions &sessions);

      /** Replays a session stored on the device.
      While a session is being replayed, this method runs a message loop which replaces loop().
      The message loop receives and dispatches real-time messages and stored session data to the callbacks.
      When a callback returns true, the session replay ends and this method returns.
      The method returns if the end of stored session was reached.
      \param startTime Session::startTime attribute value (as returned by getSessions()) of the session to replay.
      \param iniFrame Frame sequence number from which the session starts to be replayed (if ommited, start from frame 0).
      \remarks This method cannot be called from a callback. This method cannot be called while the device is acquiring.
      */
      void replaySession(time_t startTime, int iniFrame=0);

      /** Deletes all sessions stored on the device.
      \remarks This method cannot be called while the device is acquiring.
      */
      void deleteAllSessions(void);

      /** Returns the amount of memory used by all sessions stored on the device in kBytes.
      \remarks This method cannot be called while the device is acquiring.
      \see \c memorySize property in getProperties().
      */
      int  getMemoryUsed(void);

      /** Raw frames callback for stored sessions replay.
      This callback is called by replaySession() for every frame to be replayed from the stored session.
      In order to receive data frames, an application must derive MemoryDev class (or any of its derived classes) to a new class
      and override this method in the new class.
   
      This callback parameters are equivalent to onRawFrame() callback parameters.
      \return Return true to exit replaySession() or false otherwise. Default callback returns false.
      */
      virtual bool onSessionRawFrame(int nSeq, const int data[]) {return false;}

      /** %Event callback for stored sessions replay.
      This callback is called by replaySession() for every event to be replayed from the stored session.
      In order to receive device events, an application must derive MemoryDev class (or any of its derived classes) to a new class
      and override this method in the new class.
      \param evt Replayed event, which can be a EvtDigInUpdate or EvtSync object.
      \return Return true to exit replaySession() or false otherwise. Default callback returns false.
      */
      virtual bool onSessionEvent(const Event &evt) {return false;}
   };
}

#endif
