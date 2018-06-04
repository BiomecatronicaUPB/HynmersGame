// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/InputDevice/Public/IInputDevice.h"
#include "KinectinputState.h"
#include "SharedPointerInternals.h"
#include "KinectManager.h"

/**
* Interface class for Biosignal plux input device
*/
class  FKinectInputModuleInputDevice : public IInputDevice
{
public:
	FKinectInputModuleInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& MessageHandler);

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
	virtual ~FKinectInputModuleInputDevice();
private:



	// ToDo: Is this necessary?
	bool KinectActive = false;

	TSharedRef< FGenericApplicationMessageHandler > MessageHandler;


	FKinectControllerState* KinectControllerStates;
	
	class UKinectSensor* KinectSensor = nullptr;
	UKinectManager KinectManager;
};
