#pragma once

#include "InputCoreTypes.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"

/**
* Digital weight sensors on the Wii Balance Board
*/


enum class EPluxControllerAxes
{
	thighl = 0,
	kneel,
	thighr,
	kneer,
	/** Total number of weight axes */
	TotalAxisCount
};

struct FPluxControllerKey
{
	static const FKey ThighL;
	static const FKey KneeL;
	static const FKey ThighR;
	static const FKey KneeR;
	static const FKey TimeStamp;
};

struct FPluxControllerKeyNames
{
	typedef FName Type;

	static const FName ThighL;
	static const FName KneeL;
	static const FName ThighR;
	static const FName KneeR;
	static const FName TimeStamp;
};

/**
* Capacitive Axis State
*/
struct FPluxControllerSensorState
{
	/** The axis that this sensor state maps to */
	FName Axis;

	/** The zero value (no additional weight) */
	unsigned short int Zero;

	/** What is the current sensor reading, from 0.f to 1.f */
	float Scale;

	/** What is the current sensor reading, from 0.f to 1.f */
	unsigned short int State;

	FPluxControllerSensorState()
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

struct FPluxControllerTimeStampData
{
	FName Axis;
	float Scale;
	FPluxControllerTimeStampData() : Axis(NAME_None), Scale(-1.f) {}
};

/**
* Input state for an pseudo controller
*/
struct FPluxControllerState
{
	/** Weight axes */
	FPluxControllerSensorState GonAxes[(int32)EPluxControllerAxes::TotalAxisCount];
	FPluxControllerTimeStampData TimeStamp;

	/** Default constructor */
	FPluxControllerState()
	{
		// Left leg 
		GonAxes[(int32)EPluxControllerAxes::thighl].Axis = FPluxControllerKeyNames::ThighL;
		GonAxes[(int32)EPluxControllerAxes::kneel].Axis = FPluxControllerKeyNames::KneeL;

		// Right leg
		GonAxes[(int32)EPluxControllerAxes::thighr].Axis = FPluxControllerKeyNames::ThighR;
		GonAxes[(int32)EPluxControllerAxes::kneer].Axis = FPluxControllerKeyNames::KneeR;

		TimeStamp.Axis = FPluxControllerKeyNames::TimeStamp;
	}
};