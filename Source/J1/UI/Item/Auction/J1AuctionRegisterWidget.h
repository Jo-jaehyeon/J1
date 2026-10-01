// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "J1AuctionRegisterWidget.generated.h"

class UJ1InventoryManager;
class UImage;
class UTextBlock;
class UEditableTextBox;
class UButton;
class UCheckBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnRegisterSubmitted, UJ1InventoryManager*, SourceInventory, int32, SourceSlotIndex, int64, PricePerUnit, int32, Quantity, FString, DurationHours);

/**
 * 물품 등록 UI. 열려있는 동안 인벤토리 UI의 OnItemSelected를 구독해서
 * 클릭된 아이템의 이미지/이름을 자동으로 채운다.
 * 사용법: 위젯 열 때 BindToInventory(InventoryWidget)를 호출하고,
 * 닫을 때 UnbindFromInventory()로 반드시 구독 해제할 것.
 */
UCLASS()
class J1_API UJ1AuctionRegisterWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

public:
	// InventoryWidget의 OnItemSelected에 이 위젯을 구독시킨다.
	// (타입은 UJ1InventoryWidget이지만 순환 include를 피하려고 UUserWidget으로 받아 캐스팅한다)
	UFUNCTION(BlueprintCallable, Category = "Auction")	void BindToInventory(UUserWidget* InInventoryWidget);
	UFUNCTION(BlueprintCallable, Category = "Auction")	void UnbindFromInventory();
	UFUNCTION(BlueprintCallable, Category = "Auction")	void ClosePanel();

public:
	// 서버 응답이 도착했을 때 외부(네트워크 콜백 등)에서 호출.
	UFUNCTION(BlueprintCallable, Category = "Auction")	void HandleRegisterResult(bool bSuccess);
	UFUNCTION(BlueprintCallable, Category = "Auction")	void OnLowestPriceReceived(int64 LowestPrice);

protected:
	// 제출 중/실패 시 시각 표현(버튼 텍스트, 로딩 스피너, 에러 메시지 등)
	UFUNCTION(BlueprintImplementableEvent, Category = "Auction")
	void OnSubmittingStateChanged(bool bIsSubmitting);

	UFUNCTION(BlueprintImplementableEvent, Category = "Auction")
	void OnRegisterFailed();

private:
	UFUNCTION()	void HandleItemSelected(UJ1InventoryManager* InInventory, int32 SlotIndex);
	UFUNCTION()	void HandleRegisterClicked();
	UFUNCTION() void HandleCancelClicked();
	UFUNCTION() void HandleDurationToggleChanged(bool bIsChecked);
	UFUNCTION() void HandleQuantityChanged(const FText& NewText);
	UFUNCTION() void HandlePriceChanged(const FText& NewText);

	void RequestLowestPrice(int32 ItemTemplateID);
	void UpdateTotalPrice();

public:
	// ═════════════════════
	//		 DELEGATE
	// ═════════════════════
	UPROPERTY(BlueprintAssignable, Category = "Auction")
	FOnRegisterSubmitted OnRegisterSubmitted;

protected:
	// ════════════════════════════════════
	//              UMG 바인딩
	// ════════════════════════════════════
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UImage*				Img_Icon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*			Txt_Name;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*			Txt_LowestPrice;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*			Txt_TotalPrice;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UCheckBox*			Check_Duration;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UEditableTextBox*	Input_Quantity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UEditableTextBox*	Input_Price;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*			Btn_Register;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*			Btn_Cancel;

private:
	TWeakObjectPtr<UUserWidget>			BoundInventoryWidget;
	TWeakObjectPtr<UJ1InventoryManager> SelectedInventory;
	
	int32	SelectedSlotIndex = INDEX_NONE;
	int32	SelectedItemMaxQuantity = 1;
	FString	SelectedDurationHours = "24";
	bool	bSelectedItemIsEquipment = false;
	bool	bIsRegist = false;
};