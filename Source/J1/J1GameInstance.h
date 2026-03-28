// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "J1.h"
#include "J1GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class J1_API UJ1GameInstance : public UGameInstance
{
	GENERATED_BODY()

	// GameServer
public:
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

private:
	PacketSessionRef GameSession;
};
