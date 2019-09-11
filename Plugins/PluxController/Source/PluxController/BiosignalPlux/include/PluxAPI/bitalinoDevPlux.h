/**
 * \file
 * \copyright  Copyright 2015-2016 PLUX - Wireless Biosignals, S.A.
 * \version    1.3
 * \date       March 2016
 */

#ifndef _BITALINOPLUXHEADER_
#define _BITALINOPLUXHEADER_

namespace Plux
{
   /// Base class for Plux BITalino devices.
   class BITalinoDev : public SignalsDev
   {
   public:
      /// Current device state returned by BITalinoDev::getState()
      struct State
      {
         int   analog[6],     ///< Array of analog inputs values (0...1023).
               battery,       ///< Battery voltage value (0...1023).
               batThreshold;  ///< Low-battery LED threshold (last value set with BITalinoDev::setBatThreshold()).
         /// Array of digital ports states (false for low level or true for high level).
         /// The array contents are: I1 I2 O1 O2.
         bool  digital[4];
      };

      /** Connects to a BITalino device.
      \param path Path to the BITalino device. It has the same meaning as in BaseDev::BaseDev().
      */
      BITalinoDev(const String &path);

      /** Promotes a BaseDev instance to BITalinoDev.
      \param baseDev Instance to be promoted. If promotion is successful, \c baseDev is no longer valid.
      */
      BITalinoDev(BaseDev &baseDev);

      /** Assigns the digital outputs states.
       * \param[in] output Vector of booleans to assign to digital outputs, starting at first output (O1).
       * On each vector element, false sets the output to low level and true sets the output to high level.
       * The vector must contain exactly 4 elements for original BITalino (4 digital outputs)
       * or exactly 2 elements for BITalino 2 (2 digital outputs).
       * \remarks This method must be called only during an acquisition on original BITalino. On BITalino 2 there is no restriction.
       */
      void setDOut(const Bools &output);

      /** Sets the battery voltage threshold for the low-battery LED.
       * \param[in] threshold Battery voltage threshold.
       * Parameter value | Voltage Threshold
       * --------------- | -----------------
       *               0 |   3.4 V
       *            ...  |   ...
       *              63 |   3.8 V
       * \remarks This method cannot be called during an acquisition.
       */
      void setBatThreshold(int threshold);

      /** Assigns the analog (PWM) output value (BITalino 2 only).
       * \param[in] value Analog output value to set (0...255).
       * The analog output voltage is given by: V (in Volts) = 3.3 * (value+1)/256
       */
      void setPWM(int value);

      /** Returns current device state (BITalino 2 only).
       * \remarks This method cannot be called during an acquisition.
       */
      State getState(void);
   };
}

#endif
