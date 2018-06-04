#pragma once

#include "../Public/IKinectInputModule.h"
#include "Runtime/InputDevice/Public/IInputDevice.h"

class FKinectInputModule : public IKinectInputModule
{
public:

	virtual void StartupModule() override;

	/** IPseudoControllerInterface implementation */
	virtual TSharedPtr< class IInputDevice > CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler);

	//virtual void StartupModule() OVERRIDE; // This is not required as IInputDeviceModule handels it!
	virtual void ShutdownModule() override;

	TSharedPtr< class FKinectInputModuleInputDevice >KinectInputDevice;
};