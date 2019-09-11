/**
 * \file
 * \copyright  Copyright 2015-2016 PLUX - Wireless Biosignals, S.A.
 * \version    1.3
 * \date       March 2016
 */

#ifndef _STIMPLUXHEADER_
#define _STIMPLUXHEADER_

namespace Plux
{ 
   /// This class is not covered in this documentation.
   class StimDev : public BaseDev
   {
   /// \cond
   public:
      StimDev(const String &path);
      StimDev(BaseDev &baseDev);
      
      void  startSession(int stateChanges = 0);
      void  stop(void);
      void  startMode(int mode, float time = 0);
      void  setWaveOnMode(const float *wave, int mode, bool isVoltage = false);
      void  setFrequencyOnMode(int freq, int mode);
      void  setTimeOnState(float time, int state);
      void  setModeOnState(int state, int mode);
      void  setMaxStateChanges(int maxChanges);
      void  setNextStateOnState(int state, int nextState);
      void  assignTriggerToMode(int mode);
      void  outputUnitPulse(int mode);
      void  setHVoltageState(bool HV);
      void  setCalib(int comID, int dacID, int calibVal);
   /// \endcond
   };
}

#endif
