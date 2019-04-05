// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HynmersGame.h"
#include "GameFramework/Character.h"
#include "HynmersCharacter.generated.h"

UCLASS(config=Game)
class AHynmersCharacter : public ACharacter
{
	GENERATED_BODY()

	USkeletalMeshComponent * Mesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootCameraComponent;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UHynmersCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mechanics, meta = (AllowPrivateAccess = "true"))
	bool bStartJump = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mechanics, meta = (AllowPrivateAccess = "true"))
	bool bReadyToSwim = false;

	// TODO change for VisibleAnywhere
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mechanics, meta = (AllowPrivateAccess = "true"))
	EActiveTile ActiveTile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Mechanics, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AActor> JumpPlatformClass;



public:
	AHynmersCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Landed(const FHitResult& Hit) override;

	UPROPERTY(BlueprintReadWrite, Category = "Transform")
	FTransform InitialTransform;

	UPROPERTY(BlueprintReadWrite, Category = "Transform")
	float RotationState = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mechanics)
	bool bInteractWith = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Mechanics)
	bool bCanMoveWithController = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mechanics)
	class AHynmersBaseTile* CurrentTile;


protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;
	
	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);
	void MoveForwardInput(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);
	void LookUpRate(float Rate);

	void HGJump();
	void HGStopJumping();

	UFUNCTION(BlueprintImplementableEvent)
	int32 GetCurrentTile() const;

	UFUNCTION(BlueprintImplementableEvent)
	void PostLanded(AActor* JumpPlatform);

protected:

	UFUNCTION(BlueprintCallable, Category = "Transform")
		void UpdateTransform(FTransform Target, float alpha);

	UFUNCTION(BlueprintCallable, Category = "Bones")
		FTransform GetBoneTransform(FName BoneName);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface


public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UHynmersCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EActiveTile GetActiveTile() { return ActiveTile; }
	FORCEINLINE void SetActiveTile(EActiveTile Tile) { ActiveTile = Tile; }
};

