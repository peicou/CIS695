// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "CIS695_vehicle.h"
#include "CIS695_vehicleWheelRear.h"
#include "Vehicles/TireType.h"

UCIS695_vehicleWheelRear::UCIS695_vehicleWheelRear()
{
	ShapeRadius = 18.0f;
	ShapeWidth = 15.0f;
	bAffectedByHandbrake = true;
	SteerAngle = 0.f;

	// Setup suspension forces
	SuspensionForceOffset = -4.0f;
	SuspensionMaxRaise = 8.0f;
	SuspensionMaxDrop = 12.0f;
	SuspensionNaturalFrequency = 9.0f;
	SuspensionDampingRatio = 1.05f;

	// Find the tire object and set the data for it
	static ConstructorHelpers::FObjectFinder<UTireType> TireData(TEXT("/Game/VehicleAdv/Vehicle/WheelData/Vehicle_BackTireType.Vehicle_BackTireType"));
	TireType = TireData.Object;
}
