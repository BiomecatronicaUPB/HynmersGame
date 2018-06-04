// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "PluxControllerModule.h"
#include "PluxControllerModuleInputDevice.h"

#include "Internationalization.h"
#include "Engine.h"




#define LOCTEXT_NAMESPACE "FPluxControllerModule"
DEFINE_LOG_CATEGORY_STATIC(PluxControllerPlugin, Log, All);

void FPluxControllerModule::StartupModule()
{
	IInputDeviceModule::StartupModule();
	FPluxControllerModuleInputDevice::PreInit();
}

// This function is called by *Application.cpp after startup to instantiate the modules InputDevice
TSharedPtr<class IInputDevice> FPluxControllerModule::CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	UE_LOG(PluxControllerPlugin, Log, TEXT("Creating Input Device"));

	FPluxControllerModule::PluxInputDevice = MakeShareable(new FPluxControllerModuleInputDevice(InMessageHandler));

	return PluxInputDevice;
}

void FPluxControllerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPluxControllerModule, PluxController)