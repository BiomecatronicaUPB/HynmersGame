/**
 * \file
 * \copyright  Copyright 2015-2016 PLUX - Wireless Biosignals, S.A.
 * \version    1.3
 * \date       March 2016
 */

#ifndef _EXCEPTIONPLUXHEADER_
#define _EXCEPTIONPLUXHEADER_

namespace Plux
{
   /** Generic PLUX C++ API exception.
   This exception is the base class for all specific exceptions.
   Catch this exception class to ensure that all PLUX C++ API exceptions are caught.
   */
   class Exception
   {
   public:
      /// Returns the exception description string.
      virtual const char* getDescription(void) const
      {
         return "Generic PLUX exception.";
      }
   };

   /** Base class for all notification exceptions.
   A notification exception is thrown when an external condition is preventing the API from succeeding its current operation.
   */
   class NotifException : public Exception {};

   /** Namespace for all notification exceptions.
   These exceptions derive from NotifException class.
   A notification exception is thrown when an external condition is preventing the API from succeeding its current operation.
   */
   namespace Notification
   {
      /// %Exception thrown if there was a problem while opening the communication port.
      class OpeningPort : public NotifException
      {
      public:
         const char* getDescription(void) const
         {
            return "The communication port does not exist or it is already being used.";
         }
      };

      /// %Exception thrown if there was a problem while initializing the communication port.
      class InitializingPort : public NotifException
      {
      public:
         const char* getDescription(void) const
         {
            return "The communication port could not be initialized.";
         }
      };

      /// %Exception thrown if no Bluetooth adapter was found while trying to connect to a Bluetooth device.
      class AdapterNotFound : public NotifException
      {
      public:
         const char* getDescription(void) const
         {
            return "No Bluetooth adapter was found.";
         }
      };

      /// %Exception thrown if the requested device path was not found.
      class DeviceNotFound : public NotifException
      {
      public:
         const char* getDescription(void) const
         {
            return "The device could not be found.";
         }
      };

      /// %Exception thrown if the connection with the device was lost.
      class ContactingDevice : public NotifException
      {
      public:
         const char* getDescription(void) const
         {
            return "The communication with the device was lost.";
         }
      };
   };

   /** Base class for all error exceptions.
   An error exception is thrown when an application programming error is preventing the API from succeeding its current operation.
   */
   class ErrorException : public Exception {};

   /** Namespace for all error exceptions.
   These exceptions derive from ErrorException class.
   An error exception is thrown when an application programming error is preventing the API from succeeding its current operation.
   */
   namespace Error
   {
      /// %Exception thrown if a method call has an invalid parameter value.
      class InvalidParameter : public ErrorException
      {
      public:
         const char* getDescription(void) const
         {
            return "Invalid parameter.";
         }
      };

      /// %Exception thrown if the requested operation cannot be completed due to current device state.
      class InvalidOperation : public ErrorException
      {
      public:
         /// \cond
         const int code;
         InvalidOperation(int _code = 0) : code(_code) {}
         /// \endcond

         const char* getDescription(void) const
         {
            return "Invalid operation on current device state.";
         }
      };

      /// %Exception thrown if the requested operation is not supported by the device.
      class NotSupported : public ErrorException
      {
      public:
         const char* getDescription(void) const
         {
            return "Operation not supported by the device.";
         }
      };

      /** %Exception thrown if an invalid object instance method was called.
      A BaseDev object instance becomes invalid when it is successfully promoted to a derived class.
      \see SignalsDev::SignalsDev(BaseDev &baseDev), MemoryDev::MemoryDev(BaseDev &baseDev)
      */
      class InvalidInstance : public ErrorException
      {
      public:
         const char* getDescription(void) const
         {
            return "The object instance is invalid.";
         }
      };

      /// %Exception thrown if an API module is missing (DLL or .so file).
      class MissingModule : public ErrorException
      {
      public:
         const String module; ///< Filename of the missing module.
         MissingModule(const String &_module = "") : module(_module) {} ///< %Exception constructor.

         const char* getDescription(void) const
         {
            return "An API module needed to complete the operation is missing.";
         }
      };
   };
}

#endif
