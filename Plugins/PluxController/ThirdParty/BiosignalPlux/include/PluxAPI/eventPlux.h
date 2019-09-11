/**
 * \file
 * \copyright  Copyright 2015-2016 PLUX - Wireless Biosignals, S.A.
 * \version    1.3
 * \date       March 2016
 */

#ifndef _EVENTPLUXHEADER_
#define _EVENTPLUXHEADER_

namespace Plux
{
   /// %Event timestamp class.		
   struct Clock
   {
      /// %Clock source type enumeration.
      enum Source
      {
         None,       ///< No timestamp value.
         RTC,        ///< Device real-time clock timestamp.
         FrameCount, ///< Acquisition frame counter timestamp.
         /// \cond
         Bluetooth,
         //DigAccel,
         /// \endcond
      };
      
      Source source; ///< %Clock source for this timestamp.
      int value;     ///< Timestamp value.
   };

   /// %Event base class.
   struct Event
   {
      /// %Event type enumeration.
      enum Type
      {
      // BaseDev events

      // SignalsDev events
         TypeDigInUpdate = 3,    ///< %Event is a EvtDigInUpdate object.
         TypeSchedChange = 4,    ///< %Event is a EvtSchedChange object.
         TypeSync = 5,           ///< %Event is a EvtSync object.
         TypeGestFeatures = 7,   ///< %Event is a EvtGestFeatures object.
         TypeDisconnect = 8,     ///< %Event is a EvtDisconnect object.
         TypeSignalGood = 9,     ///< %Event is a EvtSignalGood object.
         TypeBattery = 10,       ///< %Event is a EvtBattery object.

      // internal use only
         /// \cond
         TypeTest = 0x70,
         /// \endcond
      };
      
      Type type;  ///< %Event type.
   };

// BaseDev events

// SignalsDev events
   /// Digital port input change event class.
   struct EvtDigInUpdate : public Event
   {
      Clock timestamp;  ///< %Event timestamp.
      int   channel;    ///< The digital input which changed state, starting at zero. 
      bool  state;      ///< New state of digital port input. If true, new state is High, otherwise it is Low.
   };
   
   /// %Session schedule change event class.
   struct EvtSchedChange : public Event
   {
     /// %Schedule action enumeration.
     enum Action
      {
         SchedStarted = 0,       ///< A scheduled session has started.
         SchedEnded = 1,         ///< A scheduled session has ended.
         SchedCannotStart = 2,   ///< A scheduled session could not start.
      };
      
      Action   action;           ///< Change that occurred in a session schedule.
      time_t   schedStartTime;   ///< Schedule::startTime attribute value of the schedule (unique schedule identifier).
   };
   
   /// Synchronization event class.
   struct EvtSync : public Event
   {
      std::vector<Clock> timestamps;   ///< Synchronized timestamps.
   };

   /// Gesture features event class.
   /// This event is sent by GestureWatch devices only.
   struct EvtGestFeatures : public Event
   {
      //! \{
      int seq, length, zcrossht, macc, mean90hb, acczorientation;
      //! \}
   };

   /// Device disconnect event class.
   struct EvtDisconnect : public Event
   {
      /// Disconnect reason enumeration.
      enum Reason
      {
         Timeout = 1,         ///< Idle connection timeout (15 min) has elapsed.
         ButtonPressed = 2,   ///< Device button was pressed.
         BatDischarged = 4,   ///< Device battery is discharged.
      };
      
      Reason   reason;  ///< Reason for the device disconnection.
   };
   
   /// Sensor signal good event class.
   struct EvtSignalGood : public Event
   {
      int   port;    ///< Sensor port.
      bool  isGood;  ///< True if the sensor signal is valid; false otherwise.
   };

   /// Battery state event class.
   struct EvtBattery : public Event
   {
      float voltage,    ///< Battery voltage in Volts.
            percentage; ///< Remaining battery charge as a percentage of full capacity.
   };

// internal use only
/// \cond
   struct EvtTest : public Event
   {
      int subtype, len;
      const void *data;
   };
/// \endcond
}

#endif
