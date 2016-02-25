// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "CIS695_vehicleHud.generated.h"


UCLASS(config = Game)
class ACIS695_vehicleHud : public AHUD
{
	GENERATED_BODY()

public:
	ACIS695_vehicleHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface

};
