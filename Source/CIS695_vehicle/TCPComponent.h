// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Runtime/Networking/Public/Networking.h"
#include "TCPComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CIS695_VEHICLE_API UTCPComponent : public UActorComponent
{
	GENERATED_BODY()
	//pointer to UE4 socket object
	FSocket* SocketAsClient;
	//stores compete ip v4 address
	FIPv4Address ip;

public:
	//UPROPERTY:
	//EditAnywhere - property can be edited by property windows in editor
	//BlueprintReadWrite - can be modified in editor
	//Category - category shown in editor
	//address - ipaddress
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Connection")
		FString address;

	//port - port for ip address
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Connection")
		int32 port;

	// Sets default values for this component's properties
	UTCPComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//UFUNCTION:
	//BlueprintCallable - can be called from the blueprint editor
	//Category - category shown in editor
	//tryTCPSocket - tries to connect using information in UPROPERTY
	UFUNCTION(BlueprintCallable, Category = "TCP Connection")
		void tryTCPSocket();

	//sendMsg - sends a message through socket connection
	//str - string containing the message. The message is sent as a uchar array anyway, to be parsed at reception
	UFUNCTION(BlueprintCallable, Category = "TCP Connection")
		int32 sendMsg(const FString str);

	//endConnection - terminates connection
	UFUNCTION(BlueprintCallable, Category = "TCP Connection")
		void endConnection();

	//debugMsg - prints a debug message to the (host) screen
	//str - message to be printed
	void debugMsg(const FString str);

	//method used internally to calculate UPROPERTY tagged values on editor changes
	void CalculateValues();

	//calls CalculateValues when a change is detected in-editor
	void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent);
		
	
};

