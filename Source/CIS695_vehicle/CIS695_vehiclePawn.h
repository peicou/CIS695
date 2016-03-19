// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Wheeledvehicle.h"
#include "CIS695_vehiclePawn.generated.h"


class UPhysicalMaterial;
class UCameraComponent;
class USpringArmComponent;
class UTextRenderComponent;
class UInputComponent;
class UTCPComponent;

UCLASS(config = Game)
class ACIS695_vehiclePawn : public AWheeledVehicle
{
	GENERATED_BODY()

		/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;

	/** Camera component for the In-vehicle view */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* InternalCamera;

	/** Text component for the In-vehicle speed */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UTextRenderComponent* InvehicleSpeed;

	/** Text component for the In-vehicle gear */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UTextRenderComponent* InvehicleGear;

	/** Audio component for the engine sound */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UAudioComponent* EngineSoundComponent;

	/** FCSZ Adding TCPComponent for remote connectivity */
	UPROPERTY(Category = Network, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UTCPComponent* TCPNetComponent;
	
public:
	ACIS695_vehiclePawn();

	/** The current speed as a string eg 10 km/h */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
		FText SpeedDisplayString;

	/** The current gear as a string (R,N, 1,2 etc) */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
		FText GearDisplayString;

	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
		/** The color of the invehicle gear text in forward gears */
		FColor	GearDisplayColor;

	/** The color of the invehicle gear text when in reverse */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
		FColor	GearDisplayReverseColor;

	/** Are we using invehicle camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
		bool bInvehicleCameraActive;

	/** Are we in reverse gear */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
		bool bInReverseGear;

	/** Initial offset of invehicle camera */
	FVector InternalCameraOrigin;

	// Begin Pawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End Pawn interface

	// Begin Actor interface
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override;
	// End Actor interface

	/** Handle pressing forwards */
	void MoveForward(float Val);

	/** Setup the strings used on the hud */
	void SetupInvehicleHUD();

	/** Update the physics material used by the vehicle mesh */
	void UpdatePhysicsMaterial();

	/** Handle pressing right */
	void MoveRight(float Val);
	/** Handle handbrake pressed */
	void OnHandbrakePressed();
	/** Handle handbrake released */
	void OnHandbrakeReleased();
	/** Switch between cameras */
	void OnToggleCamera();
	/** Handle exit event */
	void OnExit();
	/** Handle Msg event */
	void OnMsg();


	static const FName LookUpBinding;
	static const FName LookRightBinding;
	static const FName EngineAudioRPM;
	///////////////////////////////////////

	void VShow(const FString str);

	//////////////////////////////

private:
	/**
	* Activate In-vehicle camera. Enable camera and sets visibility of invehicle hud display
	*
	* @param	bState true will enable in vehicle view and set visibility of various
	*/
	void EnableInvehicleView(const bool bState);

	/** Update the gear and speed strings */
	void UpdateHUDStrings(float dt);

	/* Are we on a 'slippery' surface */
	bool bIsLowFriction;
	/** Slippery Material instance */
	UPhysicalMaterial* SlipperyMaterial;
	/** Non Slippery Material instance */
	UPhysicalMaterial* NonSlipperyMaterial;

	float dTime;

public:
	/** Returns SpringArm subobject **/
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	/** Returns InternalCamera subobject **/
	FORCEINLINE UCameraComponent* GetInternalCamera() const { return InternalCamera; }
	/** Returns InvehicleSpeed subobject **/
	FORCEINLINE UTextRenderComponent* GetInvehicleSpeed() const { return InvehicleSpeed; }
	/** Returns InvehicleGear subobject **/
	FORCEINLINE UTextRenderComponent* GetInvehicleGear() const { return InvehicleGear; }
	/** Returns EngineSoundComponent subobject **/
	FORCEINLINE UAudioComponent* GetEngineSoundComponent() const { return EngineSoundComponent; }
	/** FCSZ Returns TCPComponent subobject **/
	FORCEINLINE UTCPComponent* GetTCPComponent() const { return TCPNetComponent; }
};
