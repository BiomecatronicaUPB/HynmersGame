#pragma once


#include "CoreMinimal.h"
#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformTypes.h"
#endif
#include <windows.h>
#include <ole2.h>
#include "NuiApi.h"
#if PLATFORM_WINDOWS
#include "HideWindowsPlatformTypes.h"
#endif


DEFINE_LOG_CATEGORY_STATIC(KinectLog, Log, All);

class UKinectManager
{

public:
	// Sets default values for this actor's properties
	UKinectManager();

	/** Initialize function, should be called after properties are set */
	TArray<class UKinectSensor*> GetSensor();

	void EnumerateSensors();

	void UpdateMainWindow(PCWSTR instanceName, HRESULT sensorStatus);

	//TMap<FName, class UKinectSensor*> m_sensorMap;
	TArray<class UKinectSensor*> m_sensorArray;

	

};