#pragma once

//#include "AllowWindowsPlatformTypes.h"
//#include "NuiImageBuffer.h"
//#include "HideWindowsPlatformTypes.h"

#include "KinectManager.h"
#include "KinectinputState.h"
#include "KinectSensor.generated.h"

#define MAX_PLAYER_INDEX    6

enum DEPTH_TREATMENT
{
	CLAMP_UNRELIABLE_DEPTHS,
	TINT_UNRELIABLE_DEPTHS,
	DISPLAY_ALL_DEPTHS,
};


USTRUCT(BlueprintType)
struct FSkelStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "kinect Skeleton Struct")
		TArray<FVector> JointPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "kinect Skeleton Struct")
		TArray<FQuat> rotationAbsoluteQuaternion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "kinect Skeleton Struct")
		TArray<FRotator> rotationHierarchicalQuaternion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "kinect Skeleton Struct")
		uint8 PlayerID;

		void SetSkelJointPosition(TArray<FVector> NewJointPosition)
		{
			JointPosition = NewJointPosition;
		}
		void SetSkelRotationAbsoluteQuaternion(TArray<FQuat> NewRotationAbsoluteQuaternion)
		{
			rotationAbsoluteQuaternion = NewRotationAbsoluteQuaternion;
		}

		void SetSkelRotationHierarchicalQuaternion(TArray<FRotator> NewRotationHierarchicalQuaternion)
		{
			rotationHierarchicalQuaternion = NewRotationHierarchicalQuaternion;
		}

		void SetPlayerID(int NewPlayerID)
		{
			PlayerID = (uint8)NewPlayerID;
		}

		FSkelStruct(){

		}

};

class UKinectSensor
{

public:
	static HANDLE                  m_pColorStreamHandle;
	static HANDLE                  m_hNextColorFrameEvent;
	static HANDLE                  m_pDepthStreamHandle;
	static HANDLE                  m_hNextDepthFrameEvent;
	static HANDLE                  m_pSkeletonStreamHandle;
	static HANDLE                  m_hNextSkeletonEvent;

	BYTE*                   m_depthRGBX;
	static const int        cDepthWidth = 640;
	static const int        cDepthHeight = 480;
	static const int        cBytesPerPixel = 4;

	FSkelStruct Skeletons;
	
	
	UKinectSensor();

	//virtual void BeginDestroy() override;
	
	BYTE GetIntensity(int depth);
	inline void SetColor(unsigned int* pColor, BYTE red, BYTE green, BYTE blue, BYTE alpha = 255);
	BYTE* ResetBuffer(unsigned int size);
	void InitDepthColorTable();
	void CopyDepth(const BYTE* source, unsigned int size, BOOL nearMode, DEPTH_TREATMENT treatment);
	
	void CloseKinect();

	void Init(INuiSensor* pNuiSensor);

	void InitializeSensor();
	
	FVector4 GetAcelerometro();

	void UpdateColor();

	void UpdateDepth();

	void UpdateSkeleton(FKinectControllerState * KinectControllerStates);


	INuiSensor* mNuiSensor = nullptr;
	
	
	void ProcessColor();
	void ProcessDepth();
	void ProcessSkeleton(FKinectControllerState * KinectControllerStates);

private:

	HRESULT OpenColorStream();
	HRESULT OpenDepthStream();
	HRESULT OpenSkeletonStream();

	bool            m_nearMode;

	DEPTH_TREATMENT m_depthTreatment;

	static const BYTE    m_intensityShiftR[MAX_PLAYER_INDEX + 1];
	static const BYTE    m_intensityShiftG[MAX_PLAYER_INDEX + 1];
	static const BYTE    m_intensityShiftB[MAX_PLAYER_INDEX + 1];
	unsigned int         m_depthColorTable[MAX_PLAYER_INDEX + 1][USHRT_MAX + 1];
	unsigned long               m_nSizeInBytes;
};

