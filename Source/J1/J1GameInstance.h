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
	
	// Server
public:
	UFUNCTION(BlueprintCallable)
	void ConnectToLoginServer();

	UFUNCTION(BlueprintCallable)
	void RequeseDisconnect(ESessionType sessionType);

	void Disconnect(ESessionType sessionType);

	void SendPacket(ESessionType sessionType, asio::mutable_buffer& buffer);

private:
	SessionPtr FindTargetSession(ESessionType sessionType);

private:
	SessionPtr loginSession;
	SessionPtr gameSession;
	SessionPtr chatSession;
	bool bLeaveConfirmed = false;

public:
	// Delegate
	FOnChatReceived OnChatReceived;
	FOnLoginResult OnLoginResult;
};
