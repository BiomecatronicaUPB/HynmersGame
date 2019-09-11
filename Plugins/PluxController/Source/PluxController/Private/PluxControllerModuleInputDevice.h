// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/InputDevice/Public/IInputDevice.h"
#include "PluxInputState.h"
#include "SharedPointerInternals.h"
#include "PluxControllerDevice.h"

/**
* Interface class for Biosignal plux input device
*/
class  FPluxControllerModuleInputDevice : public IInputDevice
{
public:
	FPluxControllerModuleInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& MessageHandler);

	/** Tick the interface (e.g. check for new controllers) */
	virtual void Tick(float DeltaTime) override;


	/** Initizlization of keys and device connection manage */
	static void PreInit();

	/** Poll for controller state and send events if needed */
	virtual void SendControllerEvents() override;

	/** Set which MessageHandler will get the events from SendControllerEvents. */
	virtual void SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;

	/** Exec handler to allow console commands to be passed through for debugging */
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	// IForceFeedbackSystem pass through functions
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values) override;
	virtual ~FPluxControllerModuleInputDevice();
private:

	static void FindDevices(Plux::DevInfos &devices);

	bool Connect();

	// ToDo: Is this necessary?
	bool Active;
	bool KinectActive = false;
	static bool bCorrectlyPreInit;

	TSharedRef< FGenericApplicationMessageHandler > MessageHandler;

	TSharedPtr<FPluxControllerState, ESPMode::ThreadSafe> PluxControllerStates;

	static class FPluxThread *PluxRunnable;
	class FRunnableThread* PluxThread;

	int MaxReconnectionTries = 10;
};
