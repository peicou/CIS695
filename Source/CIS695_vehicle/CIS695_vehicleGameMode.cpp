// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "CIS695_vehicle.h"
#include "CIS695_vehicleGameMode.h"
#include "CIS695_vehiclePawn.h"
#include "CIS695_vehicleHud.h"

ACIS695_vehicleGameMode::ACIS695_vehicleGameMode()
{
	DefaultPawnClass = ACIS695_vehiclePawn::StaticClass();
	HUDClass = ACIS695_vehicleHud::StaticClass();
}
