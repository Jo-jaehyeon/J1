// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "J1.h"
#include "Types/J1EnumTypes.h"
#include "J1GameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChatReceived, const FString&, const FString&, const FString&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLoginResult, ELoginMode, bool);

UCLASS()
class J1_API UJ1GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Shutdown() override;
	// GameServer
public:
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void RequeseDisconnect();

	void Disconnect();

	void SendPacket(asio::mutable_buffer& buffer);

private:
	SessionPtr GameSession;
	bool bLeaveConfirmed = false;

public:
	// Delegate
	FOnChatReceived OnChatReceived;
	FOnLoginResult OnLoginResult;
};
