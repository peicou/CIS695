// Fill out your copyright notice in the Description page of Project Settings.

#include "CIS695_vehicle.h"
#include "TCPClientActor.h"
#include <string>
#include "Engine.h"


// Sets default values
ATCPClientActor::ATCPClientActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	address = TEXT("192.168.1.81");
	port = 3492;
	SocketAsClient = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

}

// Called when the game starts or when spawned
void ATCPClientActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATCPClientActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATCPClientActor::tryTCPSocket()
{
	FIPv4Address::Parse(address, ip);

	if (SocketAsClient->GetConnectionState() != ESocketConnectionState::SCS_Connected)
	{
		TSharedPtr<FInternetAddr, ESPMode::NotThreadSafe> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		addr->SetIp(ip.GetValue());
		addr->SetPort(port);

		if (SocketAsClient->Connect(*addr))
		{
			debugMsg("Connected to remote server");
		}
	}
	else {
		debugMsg("Connection to server failed. busy port maybe?");
	}

}

int32 ATCPClientActor::sendMsg(const FString str)
{
	if (SocketAsClient->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		FString serialized = str;
		TCHAR *serializedChar = serialized.GetCharArray().GetData();
		int32 size = FCString::Strlen(serializedChar);
		int32 sent = 0;
		bool successful = SocketAsClient->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);
		if (successful) {
			debugMsg("message sent");
		}
		else {
			debugMsg("message not sent");
			return 1;
		}
	}
	else
	{
		debugMsg("No connection");
		return -1;
	}
	return 0;
}

void ATCPClientActor::endConnection()
{
	if (SocketAsClient->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		SocketAsClient->Close();
		debugMsg("Socket closed");
	}
	else {
		debugMsg("No connection to terminate");
	}
}

//Rama's String From Binary Array
FString ATCPClientActor::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	//Create a string from a byte array!
	std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());
	return FString(cstr.c_str());
}

void ATCPClientActor::debugMsg(const FString str)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, str);
	}
}

void ATCPClientActor::CalculateValues()
{
	//For editor blueprint refreshing
	FIPv4Address::Parse(address, ip);
}

#if WITH_EDITOR
void ATCPClientActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	CalculateValues();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif