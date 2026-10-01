// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "J1.h"
#include "Types/J1EnumTypes.h"
#include "Types/J1AuctionDefine.h"
#include "Types/J1CharacterCustomizeTypes.h"
#include "J1GameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChatReceived, const FString&, const FString&, const FString&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLoginResult, ELoginMode, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNotice, FString);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCheckNickName, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLobbyListChange, bool, FLobbySlotInfo&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyListSet, const TArray<FLobbySlotInfo>&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAuctionListSet, bool, const TArray<FAuctionEntry>&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnResponseRegister, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnResponseLowestPrice, int64);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAuctionReceiptListSet, const TArray<FAuctionReceiptEntry>&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnResponseReceipt, int64, bool);

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
	int32 GetCurrentLobbySlot() { return CurrentLobbySlot; }
	void SetCurrentLobbySlot(int32 InSlotIndex) { CurrentLobbySlot = InSlotIndex; }
	
	FLobbySlotInfo GetCharacterInfo() { return CurrentCharacterInfo; }
	void SetGetCharacterInfo(FLobbySlotInfo _CharacterInfo) { CurrentCharacterInfo = _CharacterInfo; }
	
public:
	// ═════════════════════
	//		 DELEGATE
	// ═════════════════════
	FOnChatReceived			OnChatReceived;
	FOnLoginResult			OnLoginResult;
	FOnNotice				OnNotice;
	FOnCheckNickName		OnCheckNickName;
	FOnLobbyListChange		OnLobbyListChange;
	FOnLobbyListSet			OnLobbyListSet;
	FOnAuctionListSet		OnAuctionListSet;
	FOnResponseRegister		OnResponseRegister;
	FOnResponseLowestPrice	OnResponseLowestPrice;
	FOnAuctionReceiptListSet	OnAuctionReceiptListSet;	// TODO: 수령함 목록 응답 패킷 핸들러에서 Broadcast
	FOnResponseReceipt		OnResponseReceipt;		// TODO: 수령 결과 응답 패킷 핸들러에서 Broadcast (ReceiptID, bSuccess)

protected:
	UPROPERTY()
	int32 CurrentLobbySlot = INDEX_NONE;

private:
	SessionPtr loginSession;
	SessionPtr gameSession;
	SessionPtr chatSession;
	bool bLeaveConfirmed = false;

	int			_userid;
	std::string _token;

	FLobbySlotInfo CurrentCharacterInfo;
};