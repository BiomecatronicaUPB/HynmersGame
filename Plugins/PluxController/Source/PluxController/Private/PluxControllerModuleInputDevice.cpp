#include "PluxControllerModuleInputdevice.h"
#include "../Public/IPluxControllerModule.h"

#include "GenericPlatformMath.h"

#include "Engine/Level.h"
#include "Engine/World.h"
#include "RunnableThread.h"
#include "SlateBasics.h"
#include "GenericApplicationMessageHandler.h"

#include "KinectSensor.h"
#include "KinectManager.h"

DECLARE_STATS_GROUP(TEXT("Kinect"), STATGROUP_Kinect, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("Kinect Tick"), STAT_KinectTick, STATGROUP_Kinect);

#define LOCTEXT_NAMESPACE "FPluxControllerModule"

const FKey FKinectControllerKey::ThighL("thigh_l_axis");
const FKey FKinectControllerKey::KneeL("knee_l_axis");
const FKey FKinectControllerKey::AnkleL("ankle_l_axis");
const FKey FKinectControllerKey::ThighR("thigh_r_axis");
const FKey FKinectControllerKey::KneeR("knee_r_axis");
const FKey FKinectControllerKey::AnkleR("ankle_r_axis");
const FKey FKinectControllerKey::TimeStamp("time_stamp_k");

const FKinectControllerKeyNames::Type FKinectControllerKeyNames::ThighL("thigh_l_axis");
const FKinectControllerKeyNames::Type FKinectControllerKeyNames::KneeL("knee_l_axis");
const FKinectControllerKeyNames::Type FKinectControllerKeyNames::AnkleL("ankle_l_axis");
const FKinectControllerKeyNames::Type FKinectControllerKeyNames::ThighR("thigh_r_axis");
const FKinectControllerKeyNames::Type FKinectControllerKeyNames::KneeR("knee_r_axis");
const FKinectControllerKeyNames::Type FKinectControllerKeyNames::AnkleR("ankle_r_axis");
const FKinectControllerKeyNames::Type FKinectControllerKeyNames::TimeStamp("time_stamp_k");


FPluxControllerModuleInputDevice::FPluxControllerModuleInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) : MessageHandler(InMessageHandler) {

	KinectControllerStates = new FKinectControllerState();

	// Initializing Kinect Sensor
	TArray<UKinectSensor*>FoundSensors =  KinectManager.GetSensor();
	if (FoundSensors.Num()) {
		KinectSensor = FoundSensors[0];
		if (KinectSensor && KinectSensor->mNuiSensor) {
			KinectSensor->InitializeSensor();
		}
	}
	else {
		UE_LOG(KinectLog, Warning, TEXT("No Kinect found"))
	}

}

void FPluxControllerModuleInputDevice::PreInit()
{

	EKeys::AddKey(FKeyDetails(FKinectControllerKey::ThighL, LOCTEXT("thigh_l_axis", "Thigh Left Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(FKinectControllerKey::KneeL, LOCTEXT("knee_l_axis", "Knee Left Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(FKinectControllerKey::AnkleL, LOCTEXT("ankle_l_axis", "Ankle Left Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(FKinectControllerKey::ThighR, LOCTEXT("thigh_r_axis", "Thigh Right Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(FKinectControllerKey::KneeR, LOCTEXT("knee_r_axis", "Knee Right Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(FKinectControllerKey::AnkleR, LOCTEXT("ankle_r_axis", "Ankle Right Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(FKinectControllerKey::TimeStamp, LOCTEXT("time_stamp_k", "Time Stamp"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));


}

void FPluxControllerModuleInputDevice::Tick(float DeltaTime) 
{
	SCOPE_CYCLE_COUNTER(STAT_KinectTick);
	// This will spam the log heavily, comment it out for real plugins :)

	if (KinectSensor && KinectSensor->mNuiSensor) {
		KinectSensor->UpdateSkeleton(KinectControllerStates);
		KinectActive = KinectSensor->Skeletons.rotationHierarchicalQuaternion.Num() == NUI_SKELETON_POSITION_COUNT;
	}

}

void FPluxControllerModuleInputDevice::SendControllerEvents() 
{
	if (KinectActive) {
		for (auto i : KinectControllerStates->DegreesAxes) {
			MessageHandler->OnControllerAnalog(i.Axis, 0, i.Scale);
		}

		MessageHandler->OnControllerAnalog(KinectControllerStates->TimeStamp.Axis, 0, KinectControllerStates->TimeStamp.Scale);
	}
	else {
		for (auto i : KinectControllerStates->DegreesAxes) {
			MessageHandler->OnControllerAnalog(i.Axis, 0, 0);
		}
	}

}

void FPluxControllerModuleInputDevice::SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) {
	UE_LOG(KinectLog, Log, TEXT("Set Message Handler"));
	MessageHandler = InMessageHandler;
}

bool FPluxControllerModuleInputDevice::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) {
	UE_LOG(KinectLog, Log, TEXT("Execute Console Command: %s"), Cmd);

	// Put your fancy custom console command code here... 
	// ToDo: use this to let you fire pseudo controller events

	return true;
}

// IForceFeedbackSystem pass through functions
//  - I *believe* this is a handel for the game to communicate back to your third party libary (i.e. game tells joystick to increase force feedback/vibrate/turn on/off a light)
void FPluxControllerModuleInputDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) {
	UE_LOG(KinectLog, Log, TEXT("Set Force Feedback %f"), Value);
}
void FPluxControllerModuleInputDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values) {
	// This will spam the log heavily, comment it out for real plugins :)
	UE_LOG(KinectLog, Log, TEXT("Set Force Feedback Values"));
}

// This is where you nicely clean up your plugin when its told to shut down!
FPluxControllerModuleInputDevice::~FPluxControllerModuleInputDevice() {
	UE_LOG(KinectLog, Log, TEXT("Closing Kinect"));
	KinectSensor->CloseKinect();
	delete KinectSensor;
}


