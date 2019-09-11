/**
 * \file
 * \copyright  Copyright 2015-2016 PLUX - Wireless Biosignals, S.A.
 * \version    1.3
 * \date       March 2016

 \mainpage
 The PLUX C++ API brings to C++ applications all the functionality of PLUX devices.
 The Plux::BaseDev class (or any of its derived classes) encapsulates a connection to a PLUX device.
 Each Plux::BaseDev-derived class provides methods to access the specific functionality of certain PLUX devices.

 The API is not thread-safe since its methods cannot be called simultaneously by different threads.
 The only exception to this rule is the Plux::BaseDev::interrupt() method.

 The API is delivered as a set of C++ header files and pre-compiled library file specific to a given platform and compiler options.

 The current API version string is available in the macro #PLUX_API_VERSION.

 The PLUX API is also available as wrapper to Python 2.7 applications.

 \namespace Plux
 The PLUX C++ API namespace
*/

#ifndef _MAINPLUXHEADER_
#define _MAINPLUXHEADER_

#include <set>
#include <map>
#include <string>
#include <vector>
#include <time.h>

#define PLUX_API_VERSION   "1.3"   ///< Current API version string
 
#include "utilPlux.h"
#include "exceptionPlux.h"
#include "eventPlux.h"
#include "baseDevPlux.h"
#include "signalsDevPlux.h"
#include "memoryDevPlux.h"
#include "stimDevPlux.h"
#include "bitalinoDevPlux.h"

#endif
