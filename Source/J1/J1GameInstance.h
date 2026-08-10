// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "J1.h"
#include "Types/J1EnumTypes.h"
#include "Types/J1CharacterCustomizeTypes.h"
#include "J1GameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChatReceived, const FString&, const FString&, const FString&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLoginResult, ELoginMode, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNotice, FString);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLobbyListChange, bool, FLobbySlotInfo&);

UCLASS()
class J1_API UJ1GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Shutdown() override;
	
	// Server
public:
	UFUNCTION(BlueprintCallable)
	void ConnectToServer(ESessionType sessionType);

	UFUNCTION(BlueprintCallable)
	void RequestDisconnect(ESessionType sessionType);
	void Disconnect(ESessionType sessionType);

	void SendPacket(ESessionType sessionType, asio::mutable_buffer& buffer);

	int GetUserid() { return _userid; }
	std::string GetLoginToken() { return _token; }
	void SetUserid(int userid) { _userid = userid; }
	void SetLoginToken(std::string token) { _token = token; }

private:
	SessionPtr FindTargetSession(ESessionType sessionType);

	// Lobby & Customize
public:
	int32 GetCurrentCharacterNum() { return CurrentCharacterNum; }
	void SetCurrentCharacterNum(int32 InSlotIndex) { CurrentCharacterNum = InSlotIndex; }
	
public:
	// ═════════════════════
	//		 DELEGATE
	// ═════════════════════
	FOnChatReceived		OnChatReceived;
	FOnLoginResult		OnLoginResult;
	FOnNotice			OnNotice;
	FOnLobbyListChange  OnLobbyListChange;

protected:
	UPROPERTY()
	int32 CurrentCharacterNum = INDEX_NONE;

private:
	SessionPtr loginSession;
	SessionPtr gameSession;
	SessionPtr chatSession;
	bool bLeaveConfirmed = false;

	int			_userid;
	std::string _token;
};
