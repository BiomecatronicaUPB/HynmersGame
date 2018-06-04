#pragma once

#include "InputCoreTypes.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"

/**
* Digital weight sensors on the Wii Balance Board
*/


enum class EKinectControllerAxes
{
	thighl= 0,
	kneel,
	anklel,
	thighr,
	kneer,
	ankler,
	/** Total number of weight axes */
	TotalAxisCount
};

struct FKinectControllerKey
{
	static const FKey ThighL;
	static const FKey KneeL;
	static const FKey AnkleL;
	static const FKey ThighR;
	static const FKey KneeR;
	static const FKey AnkleR;
	static const FKey TimeStamp;
};

struct FKinectControllerKeyNames
{
	typedef FName Type;

	static const FName ThighL;
	static const FName KneeL;
	static const FName AnkleL;
	static const FName ThighR;
	static const FName KneeR;
	static const FName AnkleR;
	static const FName TimeStamp;
};

/**
* Capacitive Axis State
*/
struct FKinectControllerSensorState
{
	/** The axis that this sensor state maps to */
	FName Axis;

	/** The zero value (no additional weight) */
	unsigned short int Zero;

	/** What is the current sensor reading, from 0.f to 1.f */
	float Scale;

	/** What is the current sensor reading, from 0.f to 1.f */
	unsigned short int State;

	FKinectControllerSensorState()
		: Axis(NAME_None)
		, Zero(900)
		, Scale(100.f)
		, State(0)
	{
	}

	float GetValue() {
		if (State < Zero) {
			return 0.f;
		}

		return (float)(State - Zero) * Scale;
	}
};

struct FKinectControllerTimeStampData
{
	FName Axis;
	float Scale;
	FKinectControllerTimeStampData() : Axis(NAME_None), Scale(-1.f) {}
};

/**
* Input state for an pseudo controller
*/
struct FKinectControllerState
{
	/** Weight axes */
	FKinectControllerSensorState DegreesAxes[(int32)EKinectControllerAxes::TotalAxisCount];
	FKinectControllerTimeStampData TimeStamp;

	/** Default constructor */
	FKinectControllerState()
	{
		// Left leg 
		DegreesAxes[(int32)EKinectControllerAxes::thighl].Axis = FKinectControllerKeyNames::ThighL;
		DegreesAxes[(int32)EKinectControllerAxes::kneel].Axis = FKinectControllerKeyNames::KneeL;
		DegreesAxes[(int32)EKinectControllerAxes::anklel].Axis = FKinectControllerKeyNames::AnkleL;

		// Right leg
		DegreesAxes[(int32)EKinectControllerAxes::thighr].Axis = FKinectControllerKeyNames::ThighR;
		DegreesAxes[(int32)EKinectControllerAxes::kneer].Axis = FKinectControllerKeyNames::KneeR;
		DegreesAxes[(int32)EKinectControllerAxes::ankler].Axis = FKinectControllerKeyNames::AnkleR;

		TimeStamp.Axis = FKinectControllerKeyNames::TimeStamp;
	}
};