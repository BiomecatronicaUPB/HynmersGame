#include "PluxControllerModuleInputdevice.h"
#include "../Public/IPluxControllerModule.h"

#include "GenericPlatformMath.h"
#include "PluxAPI/mainPlux.h"

#include "Engine/Level.h"
#include "Engine/World.h"
#include "RunnableThread.h"
#include "SlateBasics.h"
#include "GenericApplicationMessageHandler.h"


DEFINE_LOG_CATEGORY_STATIC(LogPluxControllerDevice, Log, All);
#define LOCTEXT_NAMESPACE "FPluxControllerModule"

const FKey FPluxControllerKey::ThighL("thigh_l_axis");
const FKey FPluxControllerKey::KneeL("knee_l_axis");
const FKey FPluxControllerKey::ThighR("thigh_r_axis");
const FKey FPluxControllerKey::KneeR("knee_r_axis");
const FKey FPluxControllerKey::TimeStamp("time_stamp");

const FPluxControllerKeyNames::Type FPluxControllerKeyNames::ThighL("thigh_l_axis");
const FPluxControllerKeyNames::Type FPluxControllerKeyNames::KneeL("knee_l_axis");
const FPluxControllerKeyNames::Type FPluxControllerKeyNames::ThighR("thigh_r_axis");
const FPluxControllerKeyNames::Type FPluxControllerKeyNames::KneeR("knee_r_axis");
const FPluxControllerKeyNames::Type FPluxControllerKeyNames::TimeStamp("time_stamp");

bool FPluxControllerModuleInputDevice::bCorrectlyPreInit = true;
FPluxThread* FPluxControllerModuleInputDevice::PluxRunnable = NULL;

FPluxControllerModuleInputDevice::FPluxControllerModuleInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) : Active(true), MessageHandler(InMessageHandler) {

	Active = bCorrectlyPreInit;

	PluxControllerStates = MakeShareable(new FPluxControllerState());

	//Initializing Biosignal Plux
	if (Active){

		UE_LOG(LogPluxControllerDevice, Log, TEXT("Starting PluxControllerInputDevice"));

		if (!PluxRunnable && FPlatformProcess::SupportsMultithreading()) {

			Plux::DevInfos devices;
			FindDevices(devices);

			if (!devices.size())return;

			// Preinitializing runnable to create a device
			PluxRunnable = new FPluxThread(devices.cbegin()->path.c_str(),200);

			// Shareing protected memory
			PluxRunnable->SetControllerState(PluxControllerStates);

			// Init thread
			PluxThread = FRunnableThread::Create(PluxRunnable, TEXT("Plux_Thread"), 0, TPri_BelowNormal);
		}
	}
	else {
		UE_LOG(LogPluxControllerDevice, Log, TEXT("Failed to Create Device"));
	}

}

void FPluxControllerModuleInputDevice::PreInit()
{
	Plux::DevInfos devices;
	FindDevices(devices);
	
	if (bCorrectlyPreInit) {
		if (!devices.size()) {
			bCorrectlyPreInit = false;
			UE_LOG(LogPluxControllerDevice, Log, TEXT("No Devices Found"));
		}
		else {
			UE_LOG(LogPluxControllerDevice, Log, TEXT("Found Devices"));
			for (Plux::DevInfos::const_iterator it = devices.cbegin(); it != devices.cend(); it++)
				UE_LOG(LogPluxControllerDevice, Log, TEXT("Path: %s  Description : %s"), UTF8_TO_TCHAR(it->path.c_str()), UTF8_TO_TCHAR(it->description.c_str()));
		}
	}


	// Register the FKeys (Gamepad key for controllers, Mouse for mice, FloatAxis for non binary values e.t.c.)
	EKeys::AddKey(FKeyDetails(FPluxControllerKey::ThighL, LOCTEXT("thigh_l_axis", "Thigh Left Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(FPluxControllerKey::KneeL, LOCTEXT("knee_l_axis", "Knee Left Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(FPluxControllerKey::ThighR, LOCTEXT("thigh_r_axis", "Thigh Right Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(FPluxControllerKey::KneeR, LOCTEXT("knee_r_axis", "Knee Right Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(FPluxControllerKey::TimeStamp, LOCTEXT("time_stamp", "Time Stamp"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
}

void FPluxControllerModuleInputDevice::Tick(float DeltaTime) 
{

}

void FPluxControllerModuleInputDevice::SendControllerEvents() 
{

		// Fire analog input event
	if (Active) {
		if (PluxRunnable->Dev) {
			
			for (auto i : PluxControllerStates->GonAxes)
				//MessageHandler->OnControllerAnalog(i.Axis, 0, PluxControllerStates->TimeStamp);
				MessageHandler->OnControllerAnalog(i.Axis, 0, i.Scale);

			MessageHandler->OnControllerAnalog(PluxControllerStates->TimeStamp.Axis, 0, PluxControllerStates->TimeStamp.Scale);
			return;
		}
	}

	Active = false;
	// TODO create function reconnect
	for (auto i : PluxControllerStates->GonAxes)
		MessageHandler->OnControllerAnalog(i.Axis, 0, 0);

}

void FPluxControllerModuleInputDevice::SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) {
	UE_LOG(LogPluxControllerDevice, Log, TEXT("Set Message Handler"));
	MessageHandler = InMessageHandler;
}

bool FPluxControllerModuleInputDevice::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) {
	UE_LOG(LogPluxControllerDevice, Log, TEXT("Execute Console Command: %s"), Cmd);

	// Put your fancy custom console command code here... 
	// ToDo: use this to let you fire pseudo controller events

	return true;
}

// IForceFeedbackSystem pass through functions
//  - I *believe* this is a handel for the game to communicate back to your third party libary (i.e. game tells joystick to increase force feedback/vibrate/turn on/off a light)
void FPluxControllerModuleInputDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) {
	UE_LOG(LogPluxControllerDevice, Log, TEXT("Set Force Feedback %f"), Value);
}
void FPluxControllerModuleInputDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values) {
	// This will spam the log heavily, comment it out for real plugins :)
	UE_LOG(LogPluxControllerDevice, Log, TEXT("Set Force Feedback Values"));
}

// This is where you nicely clean up your plugin when its told to shut down!
FPluxControllerModuleInputDevice::~FPluxControllerModuleInputDevice() {
	UE_LOG(LogPluxControllerDevice, Log, TEXT("Closing PluxControllerInputDevice"));

	if (PluxRunnable) {

		PluxRunnable->Stop();
		PluxThread->WaitForCompletion();
		delete PluxRunnable;
		PluxRunnable = NULL;
	}
}

void FPluxControllerModuleInputDevice::FindDevices(Plux::DevInfos & devices)
{
	try {
		devices = Plux::BaseDev::findDevices();
	}
	catch (const Plux::Error::InvalidOperation &e)
	{
		// catch Invalid Operation exception with error code
		UE_LOG(LogPluxControllerDevice, Warning, TEXT("Biosignal Plux Library Exception: %s\tcode: %d"), e.getDescription(), e.code);
		bCorrectlyPreInit = false;

	}
	catch (const Plux::Exception &e)
	{
		// catch all other PLUX exceptions
		UE_LOG(LogPluxControllerDevice, Warning, TEXT("Biosignal Plux Library Exception: %s"), UTF8_TO_TCHAR(e.getDescription()));
		bCorrectlyPreInit = false;
	}
}

bool FPluxControllerModuleInputDevice::Connect()
{
	if (!PluxRunnable && FPlatformProcess::SupportsMultithreading()) {

		Plux::DevInfos devices;
		FindDevices(devices);

		if (!devices.size())return false;

		// Preinitializing runnable to create a device
		PluxRunnable = new FPluxThread(devices.cbegin()->path.c_str(), 200);

		// Shareing protected memory
		PluxRunnable->SetControllerState(PluxControllerStates);

		// Init thread
		PluxThread = FRunnableThread::Create(PluxRunnable, TEXT("Plux_Thread"), 0, TPri_BelowNormal);
	}
	return false;
}
