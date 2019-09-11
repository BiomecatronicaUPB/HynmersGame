/**
 * \file
 * \copyright  Copyright 2015-2016 PLUX - Wireless Biosignals, S.A.
 * \version    1.3
 * \date       March 2016
 */

#ifndef _SIGNALSPLUXHEADER_
#define _SIGNALSPLUXHEADER_

namespace Plux
{
   /** Acquisition source class.
   An acquisition source is a sensor connected to a device port.
   Sampling frequency divisor and sampling resolution can be set for each source.
   Each source can have up to 8 channels.
   All source channels share the same frequency divisor and sampling resolution settings defined for the source.
   \see SignalsDev::start(float baseFreq, const Sources &sources), Schedule::sources
   */
   struct Source
   {
      /// %Source class enumeration
      enum Class
      {
         ANY,
         EMG,
         ECG,
         LIGHT,
         EDA,
         BVP,
         RESP,
         XYZ,
         SYNC,
         EEG,
         UPPER_BOUND
      };

      int   port;          ///< %Source port (1...8 for analog ports). Default value is zero.
      int   freqDivisor;   ///< %Source frequency divisor from acquisition base frequency (>= 1). Default value is 1.
      int   nBits;         ///< %Source sampling resolution in bits (8 or 16). Default value is 16.
      int   chMask;        ///< Bitmask of source channels to sample (bit 0 is channel 0, etc). Default value is 1 (channel 0 only).
      int   serialNum;     ///< %Source serial number (reserved, must be zero). Default value is zero.
      Class clas;          ///< %Source class (reserved, must be Source::ANY). Default value is Source::ANY.

      /** Initializes all structure fields.
      All arguments are optional. A structure field is initialized with the corresponding argument value, if provided.
      Otherwise, the field is initialized with its default value.
      */
      Source(int _port=0, int _freqDivisor=1, int _nBits=16, int _chMask=0x01) :
         port(_port), freqDivisor(_freqDivisor), nBits(_nBits), chMask(_chMask), serialNum(0), clas(ANY) {}
   };

   /** Vector of Source type.
   \see SignalsDev::start(float baseFreq, const Sources &sources), Schedule::sources
   */
   typedef std::vector<Source> Sources;

   /// Base class for PLUX signal-acquiring devices.
   class SignalsDev : public BaseDev
   {
   public:
      /** Connects to a PLUX device.
      \param path Path to the PLUX device. It has the same meaning as in BaseDev::BaseDev().
      */
      SignalsDev(const String &path);

      /** Promotes a BaseDev instance to SignalsDev.
      \param baseDev Instance to be promoted. If promotion is successful, \c baseDev is no longer valid.
      */
      SignalsDev(BaseDev &baseDev);

      /** Starts a real-time acquisition session.
      This method is a shortcut to start(float baseFreq, const Sources &sources).
      It can be called if all requested sources are to be sampled at the same frequency and resolution,
      and at their channel 0 only.
      \param freq Acquisition sampling frequency in Hertz.
      \param portMask Bitmask of the device ports to acquire.
      The least significant bit corresponds to port 1, the next bit to port 2, etc. See Source::port.
      \param nBits Sampling resolution in bits. It can be 8 or 16. This parameter is ignored for BITalino devices.
      */
      void start(float freq, int portMask, int nBits);

      /** Starts a real-time acquisition session.
      This method is a shortcut to start(float baseFreq, const Sources &sources).
      It can be called if all requested sources are to be sampled at the same frequency and resolution,
      and at their channel 0 only.
      \param freq Acquisition sampling frequency in Hertz.
      \param ports Ports to acquire. See Source::port.
      \param nBits Sampling resolution in bits. It can be 8 or 16. This parameter is ignored for BITalino devices.
      */
      void start(float freq, const Ints &ports, int nBits);

      /** Starts a real-time acquisition session.
      \param baseFreq Acquisition base sampling frequency in Hertz.
      \param sources Signal sources to acquire. See Source.
      \remarks This method is not supported on BITalino devices.
      */
      void start(float baseFreq, const Sources &sources);

      /** Stops a real-time acquisition session.
      \remarks Call MemoryDev::stopSessionAcq() to stop an internal acquisition session.
      */
      void stop(void);

      /** Returns the total number of acquisition channels.
      The returned value is the total number of channels across all the acquisition sources, or zero if the device is stopped.
      This value is the number of elements of the data array passed to the onRawFrame() callback.
      */
      int getNumChannels(void);

      /** Sets the digital output state.
      \param state Output state to assign. If true, output is set to High, otherwise it is set to Low.
      \remarks On BITalino devices, only the first output (O1) is assigned to the given state. The other outputs are set to Low.
      Call BITalinoDev::setDOut() to assign all digital outputs.
      */
      void setDOut(bool state);
      
      /** Raw frames callback.
      This callback is called by message loop when a real-time data acquisition frame is received from the device.
      In order to receive data frames, an application must derive SignalsDev class (or any of its derived classes) to a new class
      and override this method in the new class.
      \param nSeq Sequence number of the frame. This number is zero for the first frame of the acquisition session,
      and then incremented by one for every subsequent frame. If this number differs more than one between consecutive
      frames, it means that data frames for the missing sequence numbers were lost, possibly due to connection problems.
      \param data Frame sampling data. Each value is acquired from a channel of each requested source,
      in the same source order as given in start(float baseFreq, const Sources &sources), or in the same port order as given in
      start(float freq, const Ints &ports, int nBits), or from lower ports to higher ports as given in
      start(float freq, int portMask, int nBits). For each multi-channel source, the values are presented from lower channels
      to higher channels. The values range for 16-bit samples is 0...65535 and for 8-bit samples is 0...255 .
      The values for a source with a frequency divisor greater than one are updated only in the frames with a sequence number
      multiple of the frequency divisor value. The number of elements of this array can be obtained by calling getNumChannels().
      \return Return true to exit message loop or false otherwise. Default callback returns false.
      */
      virtual bool onRawFrame(int nSeq, const int data[]) {return false;}
   };
}

#endif
