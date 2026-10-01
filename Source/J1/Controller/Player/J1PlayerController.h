// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "Types/J1AuctionDefine.h"
#include "J1PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class J1_API AJ1PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AJ1PlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	// Lock On
	UFUNCTION(BlueprintCallable) void ToggleLockOn();
	UFUNCTION(BlueprintCallable) void EngagedLockOn();
	UFUNCTION(BlueprintCallable) void DisengagedLockOn();


	int GetPlayerId() { return player_id; }
	void SetPlayerId(int _id) { player_id = _id; }
	
protected:
	void MoveAct(const FInputActionValue& Value);
	void OnMoveCompleted(const FInputActionValue& Value);
	void JumpAct();
	void StopJumpingAct();
	void LookAct(const FInputActionValue& Value);
	void StopAct(const FInputActionValue& Value);
	void AttackAct();
	void SkillAct(const FInputActionValue& Value);
	void ShowUI(const FInputActionValue& Value);

private:
	//----------------------
	//		경매장 관련
	//----------------------
	UFUNCTION() void OpenInventoryForAuction();
	UFUNCTION() void RequestAuctionList(bool bmyList, FString searchName);
	UFUNCTION() void RequestAuctionReceiptList();
	UFUNCTION() void RequestAuctionReceipt(FAuctionReceiptEntry ReceiptEntry);
	UFUNCTION() void ConfirmAuctionPurchase(FAuctionEntry Entry, int32 Quantity);
	UFUNCTION() void SubmitAuctionRegister(UJ1InventoryManager* SourceInventory, int32 SourceSlotIndex, int64 PricePerUnit, int32 Quantity, FString DurationHours);

	void HandleAuctionListResponse(bool myList, const TArray<FAuctionEntry>& Entries);
	void HandleRegisterResponse(bool bSuccess);
	void HandleLowestPriceResponse(int64 LowestPrice);
	void HandleReceiptListResponse(const TArray<FAuctionReceiptEntry>& Receipts);
	void HandleReceiptResponse(int64 ReceiptID, bool bSuccess);

	/*
	*  Member Variable
	*/
public:
	UPROPERTY(BlueprintReadWrite) bool bLockOnEngaged;
	UPROPERTY(BlueprintReadWrite) bool bShouldRotate;

protected:
	UPROPERTY(EditAnywhere)
	ACharacter* ControlledCharacter;

	//InputMappingContext
	TObjectPtr <class UEnhancedInputLocalPlayerSubsystem> Subsystem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> IMC_Base;

	//Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_Move;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_Jump;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_Look;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_LockOn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_Attack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_Skill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_UI;

	// Widget
	UPROPERTY()	class UJ1InventoryWidget*	InventoryWidget;
	UPROPERTY()	class UJ1AuctionWidget*		AuctionWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)	TSubclassOf<UUserWidget> InventoryWidgetClass;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)	TSubclassOf<UUserWidget> AuctionWidgetClass;

public:
	TArray<UUserWidget*> OpenedWidget;

private:
	FInputModeGameOnly GameInputMode;
	FInputModeGameAndUI UIInputMode;
	int player_id = INDEX_NONE;

	// 수령 요청을 보내고 응답을 기다리는 항목들. 성공 응답이 오면 여기서 꺼내 실제 지급한다 (확정 갱신).
	TMap<int64, FAuctionReceiptEntry> PendingReceipts;
};
