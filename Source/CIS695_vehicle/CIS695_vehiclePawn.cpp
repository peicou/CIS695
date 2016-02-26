// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "CIS695_vehicle.h"
#include "CIS695_vehiclePawn.h"
#include "CIS695_vehicleWheelFront.h"
#include "CIS695_vehicleWheelRear.h"
#include "CIS695_vehicleHud.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/TextRenderComponent.h"
#include "Sound/SoundCue.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "vehicles/WheeledvehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "Engine.h"
#include <string>
//#include "Runtime/Sockets/Public/Sockets.h"


#ifdef HMD_INTGERATION
// Needed for VR Headset
#include "Engine.h"
#include "IHeadMountedDisplay.h"
#endif // HMD_INTGERATION

const FName ACIS695_vehiclePawn::LookUpBinding("LookUp");
const FName ACIS695_vehiclePawn::LookRightBinding("LookRight");
const FName ACIS695_vehiclePawn::EngineAudioRPM("RPM");

#define LOCTEXT_NAMESPACE "vehiclePawn"

ACIS695_vehiclePawn::ACIS695_vehiclePawn()
{
	// vehicle mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> vehicleMesh(TEXT("/Game/vehicleAdv/vehicle/vehicle_SkelMesh.vehicle_SkelMesh"));
	GetMesh()->SetSkeletalMesh(vehicleMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/vehicleAdv/vehicle/vehicleAnimationBlueprint"));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	// Setup friction materials
	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> SlipperyMat(TEXT("/Game/vehicleAdv/PhysicsMaterials/Slippery.Slippery"));
	SlipperyMaterial = SlipperyMat.Object;

	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> NonSlipperyMat(TEXT("/Game/vehicleAdv/PhysicsMaterials/NonSlippery.NonSlippery"));
	NonSlipperyMaterial = NonSlipperyMat.Object;

	UWheeledVehicleMovementComponent4W* vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	check(vehicle4W->WheelSetups.Num() == 4);

	// Wheels/Tyres
	// Setup the wheels
	vehicle4W->WheelSetups[0].WheelClass = UCIS695_vehicleWheelFront::StaticClass();
	vehicle4W->WheelSetups[0].BoneName = FName("PhysWheel_FL");
	vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -8.f, 0.f);

	vehicle4W->WheelSetups[1].WheelClass = UCIS695_vehicleWheelFront::StaticClass();
	vehicle4W->WheelSetups[1].BoneName = FName("PhysWheel_FR");
	vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 8.f, 0.f);

	vehicle4W->WheelSetups[2].WheelClass = UCIS695_vehicleWheelRear::StaticClass();
	vehicle4W->WheelSetups[2].BoneName = FName("PhysWheel_BL");
	vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -8.f, 0.f);

	vehicle4W->WheelSetups[3].WheelClass = UCIS695_vehicleWheelRear::StaticClass();
	vehicle4W->WheelSetups[3].BoneName = FName("PhysWheel_BR");
	vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 8.f, 0.f);

	// Adjust the tire loading
	vehicle4W->MinNormalizedTireLoad = 0.0f;
	vehicle4W->MinNormalizedTireLoadFiltered = 0.2f;
	vehicle4W->MaxNormalizedTireLoad = 2.0f;
	vehicle4W->MaxNormalizedTireLoadFiltered = 2.0f;

	// Engine 
	// Torque setup
	vehicle4W->MaxEngineRPM = 5700.0f;
	vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.0f);
	vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.0f, 500.0f);
	vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.0f, 400.0f);

	// Adjust the steering 
	vehicle4W->SteeringCurve.GetRichCurve()->Reset();
	vehicle4W->SteeringCurve.GetRichCurve()->AddKey(0.0f, 1.0f);
	vehicle4W->SteeringCurve.GetRichCurve()->AddKey(40.0f, 0.7f);
	vehicle4W->SteeringCurve.GetRichCurve()->AddKey(120.0f, 0.6f);

	// Transmission	
	// We want 4wd
	vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;

	// Drive the front wheels a little more than the rear
	vehicle4W->DifferentialSetup.FrontRearSplit = 0.65;

	// Automatic gearbox
	vehicle4W->TransmissionSetup.bUseGearAutoBox = true;
	vehicle4W->TransmissionSetup.GearSwitchTime = 0.15f;
	vehicle4W->TransmissionSetup.GearAutoBoxLatency = 1.0f;

	// Physics settings
	// Adjust the center of mass - the buggy is quite low
	UPrimitiveComponent* UpdatedPrimitive = Cast<UPrimitiveComponent>(vehicle4W->UpdatedComponent);
	if (UpdatedPrimitive)
	{
		UpdatedPrimitive->BodyInstance.COMNudge = FVector(8.0f, 0.0f, 0.0f);
	}

	// Set the inertia scale. This controls how the mass of the vehicle is distributed.
	vehicle4W->InertiaTensorScale = FVector(1.0f, 1.333f, 1.2f);

	// Create a spring arm component for our chase camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 34.0f));
	SpringArm->SetWorldRotation(FRotator(-20.0f, 0.0f, 0.0f));
	SpringArm->AttachTo(RootComponent);
	SpringArm->TargetArmLength = 125.0f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;

	// Create the chase camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	Camera->AttachTo(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	// Create In-vehicle camera component 
	InternalCameraOrigin = FVector(-34.0f, 0.0f, 50.0f);
	InternalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCamera"));
	//InternalCamera->AttachTo(SpringArm, USpringArmComponent::SocketName);
	InternalCamera->bUsePawnControlRotation = false;
	InternalCamera->FieldOfView = 90.f;
	InternalCamera->SetRelativeLocation(InternalCameraOrigin);
	InternalCamera->AttachTo(GetMesh());

	// In vehicle HUD
	// Create text render component for in vehicle speed display
	InvehicleSpeed = CreateDefaultSubobject<UTextRenderComponent>(TEXT("InvehicleSpeed"));
	InvehicleSpeed->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	InvehicleSpeed->SetRelativeLocation(FVector(35.0f, -6.0f, 20.0f));
	InvehicleSpeed->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	InvehicleSpeed->AttachTo(GetMesh());

	// Create text render component for in vehicle gear display
	InvehicleGear = CreateDefaultSubobject<UTextRenderComponent>(TEXT("InvehicleGear"));
	InvehicleGear->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	InvehicleGear->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
	InvehicleGear->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	InvehicleGear->AttachTo(GetMesh());

	// Setup the audio component and allocate it a sound cue
	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue(TEXT("/Game/vehicleAdv/Sound/Engine_Loop_Cue.Engine_Loop_Cue"));
	EngineSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSound"));
	EngineSoundComponent->SetSound(SoundCue.Object);
	EngineSoundComponent->AttachTo(GetMesh());

	// Colors for the in-vehicle gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	bIsLowFriction = false;
	bInReverseGear = false;

	SocketAsClient = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

}

void ACIS695_vehiclePawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &ACIS695_vehiclePawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ACIS695_vehiclePawn::MoveRight);
	InputComponent->BindAxis(LookUpBinding);
	InputComponent->BindAxis(LookRightBinding);

	InputComponent->BindAction("Handbrake", IE_Pressed, this, &ACIS695_vehiclePawn::OnHandbrakePressed);
	InputComponent->BindAction("Handbrake", IE_Released, this, &ACIS695_vehiclePawn::OnHandbrakeReleased);
	InputComponent->BindAction("SwitchCamera", IE_Pressed, this, &ACIS695_vehiclePawn::OnToggleCamera);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &ACIS695_vehiclePawn::OnResetVR);
	InputComponent->BindAction("Exit", IE_Pressed, this, &ACIS695_vehiclePawn::OnExit);
}

void ACIS695_vehiclePawn::MoveForward(float Val)
{
	GetVehicleMovementComponent()->SetThrottleInput(Val);

}

void ACIS695_vehiclePawn::MoveRight(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void ACIS695_vehiclePawn::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void ACIS695_vehiclePawn::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void ACIS695_vehiclePawn::OnToggleCamera()
{
	EnableInvehicleView(!bInvehicleCameraActive);
}

void ACIS695_vehiclePawn::EnableInvehicleView(const bool bState)
{
	if (bState != bInvehicleCameraActive)
	{
		bInvehicleCameraActive = bState;

		if (bState == true)
		{
			OnResetVR();
			Camera->Deactivate();
			InternalCamera->Activate();
		}
		else
		{
			InternalCamera->Deactivate();
			Camera->Activate();
		}

		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if ((PlayerController != nullptr) && (PlayerController->PlayerCameraManager != nullptr))
		{
			PlayerController->PlayerCameraManager->bFollowHmdOrientation = true;
		}

		InvehicleSpeed->SetVisibility(bInvehicleCameraActive);
		InvehicleGear->SetVisibility(bInvehicleCameraActive);
	}
}

void ACIS695_vehiclePawn::Tick(float Delta)
{
	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;

	// Update phsyics material
	UpdatePhysicsMaterial();

	// Update the strings used in the hud (invehicle and onscreen)
	UpdateHUDStrings();

	// Set the string in the invehicle hud
	SetupInvehicleHUD();

	bool bHMDActive = false;
#ifdef HMD_INTGERATION
	if ((GEngine->HMDDevice.IsValid() == true) && ((GEngine->HMDDevice->IsHeadTrackingAllowed() == true) || (GEngine->IsStereoscopic3D() == true)))
	{
		bHMDActive = true;
	}
#endif // HMD_INTGERATION
	if (bHMDActive == false)
	{
		if ((InputComponent) && (bInvehicleCameraActive == true))
		{
			FRotator HeadRotation = InternalCamera->RelativeRotation;
			HeadRotation.Pitch += InputComponent->GetAxisValue(LookUpBinding);
			HeadRotation.Yaw += InputComponent->GetAxisValue(LookRightBinding);
			InternalCamera->RelativeRotation = HeadRotation;
		}
	}

	// Pass the engine RPM to the sound component
	float RPMToAudioScale = 2500.0f / GetVehicleMovement()->GetEngineMaxRotationSpeed();
	EngineSoundComponent->SetFloatParameter(EngineAudioRPM, GetVehicleMovement()->GetEngineRotationSpeed()*RPMToAudioScale);
}

void ACIS695_vehiclePawn::BeginPlay()
{
	FTimerHandle TH;
	bool bWantInvehicle = false;
	// First disable both speed/gear displays
	bInvehicleCameraActive = false;
	InvehicleSpeed->SetVisibility(bInvehicleCameraActive);
	InvehicleGear->SetVisibility(bInvehicleCameraActive);

#ifdef HMD_INTGERATION
	// Enable in vehicle view if HMD is attached
	bWantInvehicle = GEngine->HMDDevice.IsValid()
#endif // HMD_INTGERATION

		EnableInvehicleView(bWantInvehicle);
	// Start an engine sound playing
	EngineSoundComponent->Play();

	//connect to server
	GetWorldTimerManager().SetTimer(TH, this, &ACIS695_vehiclePawn::tryTCPSocket, 3, true);
	
}

void ACIS695_vehiclePawn::OnResetVR()
{
#ifdef HMD_INTGERATION
	if (GEngine->HMDDevice.IsValid())
	{
		GEngine->HMDDevice->ResetOrientationAndPosition();
		InternalCamera->SetRelativeLocation(InternalCameraOrigin);
		GetController()->SetControlRotation(FRotator());
	}
#else
	if (SocketAsClient->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		FString serialized = TEXT("Hello from UE4|dickbutt");
		TCHAR *serializedChar = serialized.GetCharArray().GetData();
		int32 size = FCString::Strlen(serializedChar);
		int32 sent = 0;
		bool successful = SocketAsClient->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);
		if (successful) {
			VShow("message sent");
		}
		else {
			VShow("message not sent");
		}
	} else 
	{
		VShow("No connection");
	}
	
#endif // HMD_INTGERATION
}

void ACIS695_vehiclePawn::OnExit()
{
	SocketAsClient->Close();
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}

void ACIS695_vehiclePawn::UpdateHUDStrings()
{
	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int32 KPH_int = FMath::FloorToInt(KPH);

	// Using FText because this is display text that should be localizable
	SpeedDisplayString = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));

	if (bInReverseGear == true)
	{
		GearDisplayString = FText(LOCTEXT("ReverseGear", "R"));
	}
	else
	{
		int32 Gear = GetVehicleMovement()->GetCurrentGear();
		GearDisplayString = (Gear == 0) ? LOCTEXT("N", "N") : FText::AsNumber(Gear);
	}
}

void ACIS695_vehiclePawn::SetupInvehicleHUD()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if ((PlayerController != nullptr) && (InvehicleSpeed != nullptr) && (InvehicleGear != nullptr))
	{
		// Setup the text render component strings
		InvehicleSpeed->SetText(SpeedDisplayString);
		InvehicleGear->SetText(GearDisplayString);

		if (bInReverseGear == false)
		{
			InvehicleGear->SetTextRenderColor(GearDisplayColor);
		}
		else
		{
			InvehicleGear->SetTextRenderColor(GearDisplayReverseColor);
		}
	}
}

void ACIS695_vehiclePawn::UpdatePhysicsMaterial()
{
	if (GetActorUpVector().Z < 0)
	{
		if (bIsLowFriction == true)
		{
			GetMesh()->SetPhysMaterialOverride(NonSlipperyMaterial);
			bIsLowFriction = false;
		}
		else
		{
			GetMesh()->SetPhysMaterialOverride(SlipperyMaterial);
			bIsLowFriction = true;
		}
	}
}

void ACIS695_vehiclePawn::tryTCPSocket()
{
	if (SocketAsClient->GetConnectionState()!=ESocketConnectionState::SCS_Connected)
	{
		FString address = TEXT("192.168.1.81");
		int32 port = 3491;
		FIPv4Address ip;
		FIPv4Address::Parse(address, ip);


		TSharedPtr<FInternetAddr, ESPMode::NotThreadSafe> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		addr->SetIp(ip.GetValue());
		addr->SetPort(port);

		if (SocketAsClient->Connect(*addr))
		{
			VShow("Connected to remote server");
		}
	}

}

//Rama's String From Binary Array
FString ACIS695_vehiclePawn::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	//Create a string from a byte array!
	std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());
	return FString(cstr.c_str());
}

void ACIS695_vehiclePawn::VShow(const FString str)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, str);

	}
}

#undef LOCTEXT_NAMESPACE
