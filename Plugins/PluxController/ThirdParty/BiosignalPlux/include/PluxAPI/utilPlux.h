/**
 * \file
 * \copyright  Copyright 2015-2016 PLUX - Wireless Biosignals, S.A.
 * \version    1.3
 * \date       March 2016
 */

#ifndef _UTILPLUXHEADER_
#define _UTILPLUXHEADER_

namespace Plux
{
   typedef std::string        String;  ///< String type.
   typedef std::vector<int>   Ints;    ///< Vector of int type.
   typedef std::vector<bool>  Bools;   ///< Vector of bool type.

   /// Information about a device found by BaseDev::findDevices().
   struct DevInfo
   {
      /// Device path (the path to be passed to the device class constructor).
      /// \see BaseDev::BaseDev()
      String   path;

      /// Device description as returned in its \c description property.
      /// \see BaseDev::getProperties()
      String   description;
   };
   typedef std::vector<DevInfo>  DevInfos;   ///< Vector of DevInfo type.

   /// This class encapsulates a value of one of following data types: bool, int, float and #String
   /// \see Properties
   class Variant
   {
   public:
      /// Data type enumeration
      enum Type
      {
         TypeNone,   ///< No data
         TypeBool,   ///< bool data type
         TypeInt,    ///< int data type
         TypeFloat,  ///< float data type
         TypeString, ///< #String data type
      };
      
      const Type type;  ///< Data type

      union
      {
         bool   b;
         int    i;
         float  f;
         String *s;
      }; ///< Data value

      Variant(void)     : type(TypeNone) , i(0) {}    ///< Constructs an empty %Variant.
      Variant(bool  _b) : type(TypeBool) , b(_b) {}   ///< Constructs a %Variant with a bool value.
      Variant(int   _i) : type(TypeInt)  , i(_i) {}   ///< Constructs a %Variant with an int value.
      Variant(float _f) : type(TypeFloat), f(_f) {}   ///< Constructs a %Variant with a float value.
      Variant(const char *_s) : type(TypeString), s(new String(_s)) {}     ///< Constructs a %Variant with a #String value from a const char*.
      Variant(const String &_s) : type(TypeString), s(new String(_s)) {}   ///< Constructs a %Variant with a #String value.

      /// Constructs a %Variant as a copy of another %Variant.
      Variant(const Variant &v) : type(v.type)
      {
         if (v.type == TypeString)
            s = new String(*v.s);
         else
            i = v.i;
      }

      ~Variant(void) {if (type==TypeString) delete s;}
   };

   /// Map from #String keywords to Variant types.
   /// \see BaseDev::getProperties(), SourceEx::properties
   typedef std::map<String, Variant>  Properties;
}

#endif
