// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "KinectInputModule.h"
#include "KinectInputModuleInputDevice.h"

#include "Internationalization.h"
#include "Engine.h"




#define LOCTEXT_NAMESPACE "FKinectInputModule"
DEFINE_LOG_CATEGORY_STATIC(KinectInputPlugin, Log, All);

void FKinectInputModule::StartupModule()
{
	IInputDeviceModule::StartupModule();
	FKinectInputModuleInputDevice::PreInit();
}

// This function is called by *Application.cpp after startup to instantiate the modules InputDevice
TSharedPtr<class IInputDevice> FKinectInputModule::CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	UE_LOG(KinectInputPlugin, Log, TEXT("Creating Input Device"));

	FKinectInputModule::KinectInputDevice = MakeShareable(new FKinectInputModuleInputDevice(InMessageHandler));

	return KinectInputDevice;
}

void FKinectInputModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FKinectInputModule, KinectInput)