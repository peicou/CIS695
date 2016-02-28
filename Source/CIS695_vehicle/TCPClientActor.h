// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Networking/Public/Networking.h"
#include "TCPClientActor.generated.h"

UCLASS()
class CIS695_VEHICLE_API ATCPClientActor : public AActor
{
	GENERATED_BODY()

private:
	FSocket* SocketAsClient;
	FIPv4Address ip;

public:	
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCP Connection")
	FString address;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Connection")
	int32 port;
	
	
	// Sets default values for this actor's properties
	ATCPClientActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
	UFUNCTION(BlueprintCallable, Category="TCP Connection")
	void tryTCPSocket(); 

	UFUNCTION(BlueprintCallable, Category = "TCP Connection")
	int32 sendMsg(const FString str);

	UFUNCTION(BlueprintCallable, Category = "TCP Connection")
	void endConnection();

	//Rama's StringFromBinaryArray
	FString StringFromBinaryArray(const TArray<uint8>& BinaryArray);

	void debugMsg(const FString str);

	void CalculateValues();

	void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent);
	
};
