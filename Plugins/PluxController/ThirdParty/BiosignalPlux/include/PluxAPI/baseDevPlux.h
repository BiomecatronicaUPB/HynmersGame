/**
 * \file
 * \copyright  Copyright 2015-2016 PLUX - Wireless Biosignals, S.A.
 * \version    1.3
 * \date       March 2016
 */

#ifndef _BASEPLUXHEADER_
#define _BASEPLUXHEADER_

namespace Plux
{     
/** Base class for all PLUX devices.
This class provides the basic functionality common to all PLUX devices.
A BaseDev device can be instantiated if a specific device functionality is not required.
A BaseDev instance can be promoted afterwards to a derived class instance, based of device properties.
This promotion is done by passing the BaseDev instance to the constructor of the derived class.

Alternatively, a derived class can be directly instantiated from the path string if a specific device functionality is required.

It is not possible to demote a derived class instance to a BaseDev instance.
*/
   class BaseDev
   {
   public:
      /** Finds PLUX devices within the given domain.
      \param domain Domain of search (optional). It can be "BTH" for a Classic Bluetooth search,
      "BLE" for a Bluetooth Low Energy search, or "USB" for an USB search.
      If it is not given (or if it is an empty string), a search is attempted in all domains.
      */
      static DevInfos findDevices(const String &domain = "");

      /** Connects to a PLUX device.
      \param path Path to the PLUX device. It can be a Classic Bluetooth MAC address ("xx:xx:xx:xx:xx:xx" or
      "BTHxx:xx:xx:xx:xx:xx"), a Bluetooth Low Energy MAC address ("BLExx:xx:xx:xx:xx:xx"), a serial port ("COMx")
      or an USB connection ("USB" or "USBx").
      */
      BaseDev(const String &path);

      /** Disconnects from the device.
      If the device is in real-time acquisition (through SignalsDev.start()), it is automatically stopped.
      */
      virtual ~BaseDev(void);

      /** Returns the device properties.
      The device properties map #String keywords to Variant values. The currently defined keywords are:
      Keyword        | Meaning
      -------------- | -------
      \c description | Device description string
      \c fwVersion   | Device firmware version
      \c hwVersion   | Device hardware version (not present in old devices)
      \c memorySize  | Device internal memory size in kBytes (only on MemoryDev instances) \n (zero if internal memory is not available)
      \c path        | Device path string as passed to constructor
      \c productID   | Device product identifier
      \c uid         | Device unique identifier string
   
      The \c fwVersion, \c hwVersion and \c productID properties are composed of two numbers (the major and minor numbers).
      These properties and are coded as 2-byte integers where the higher byte is the major number
      and the lower byte is the minor number.
      */
      Properties  getProperties(void);

      /// \cond
      void setParameter(int port, int index, const void *data, int dataLen);
      int  getParameter(int port, int index, void *data, int maxLen);

      void reset(void);
      /// \endcond

      /** Returns the remaining battery charge as a percentage of full capacity.
      The returned value is -1 if the device is charging.
      */
      float getBattery(void);

      /** Sets the receiving timeout value for loop().
      \param timeout Timeout value in milliseconds (optional).
      If \c timeout is 0, an immediate timeout condition occurs in loop() if there are no pending messages from device to dispatch.
      If \c timeout is not given or if it is -1, a timeout condition never occurs.
      \see onTimeout()
      */
      void setTimeout(int timeout = -1);

      /** Runs the device message loop.
      The message loop receives and dispatches the messages from the device to the callbacks.
      This method returns when a callback returns True.
      \remarks This method cannot be called from a callback.
      */
      void loop(void);

      /** Sends an interrupt signal to loop().
      This method can be called from a thread while loop() is running on another thread.
      The onInterrupt() callback will be called from loop() (in the context of its thread).
      \param param An optional parameter to be passed to onInterrupt() callback.
      \remarks This method cannot be called from a callback, and it cannot be called if loop() is not running in another thread.
      */
      void interrupt(void *param = NULL);

      /** Event callback.
      This callback is called by message loop when an event is received from the device.
      In order to receive device events, an application must derive BaseDev class (or any of its derived classes) to a new class
      and override this method in the new class.
      \param evt Received event, which can be a EvtDigInUpdate, EvtDisconnect, EvtSchedChange or EvtSync object.
      \return Return true to exit message loop or false otherwise. Default callback returns false.
      */
      virtual bool onEvent(const Event &evt) {return false;}

      /** Timeout callback.
      This callback is called by message loop when a timeout condition occurred.
      In order to receive timeout conditions, an application must derive BaseDev class (or any of its derived classes) to a new class
      and override this method in the new class.
      \return Return true to exit message loop or false otherwise. Default callback returns false.
      \see setTimeout()
      */
      virtual bool onTimeout(void) {return false;}

      /** Interrupt signal callback.
      This callback is called by message loop after interrupt() is called from another thread.
      In order to receive interrupt signals, an application must derive BaseDev class (or any of its derived classes) to a new class
      and override this method in the new class.
      \param param Optional parameter passed to interrupt() (or NULL if no parameter was given).
      \return Return true to exit message loop or false otherwise. Default callback returns false.
      */
      virtual bool onInterrupt(void *param) {return false;}


   // internal use only
      /// \cond
      struct X;
      X *x;

   protected:
      BaseDev(BaseDev &original);

   private:
      BaseDev& operator=(const BaseDev&); // assignments are not allowed
      /// \endcond
   };
}

#endif
