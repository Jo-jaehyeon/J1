// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Types/J1AuctionDefine.h"
#include "J1AuctionPurchaseWidget.generated.h"

class UImage;
class UTextBlock;
class UEditableTextBox;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPurchaseConfirmed, FAuctionEntry, Entry, int32, Quantity);

/**
 * "이 아이템을 구매하시겠습니까?" 확인 UI.
 * 메인 경매장 위젯에서 선택된 엔트리를 넘겨받아 SetEntry()로 채운다.
 */
UCLASS()
class J1_API UJ1AuctionPurchaseWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Auction")	void SetEntry(const FAuctionEntry& InEntry);	
	UFUNCTION(BlueprintCallable, Category = "Auction")	void OnLowestPriceReceived(int64 LowestPrice);
	UFUNCTION(BlueprintCallable, Category = "Auction")	void ClosePanel();

private:
	void RequestLowestPrice(int32 ItemTemplateID);
	void UpdateEstimatedPrice();
	void SetBuyQuantity(int32 NewQuantity);


	UFUNCTION()	void HandleQuantityTextChanged(const FText& NewText);
	UFUNCTION()	void HandleMaxClicked();
	UFUNCTION()	void HandleConfirmClicked();
	UFUNCTION()	void HandleCancelClicked();
	
public:
	// ═════════════════════
	//		 DELEGATE
	// ═════════════════════
	// 실제 구매 확정(서버 호출)은 이 델리게이트를 구독하는 쪽에서 처리.
	UPROPERTY(BlueprintAssignable, Category = "Auction")
	FOnPurchaseConfirmed OnPurchaseConfirmed;

protected:
	// ════════════════════════════════════
	//              UMG 바인딩
	// ════════════════════════════════════
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UImage*				Img_Icon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*			Txt_Name;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*			Txt_Count;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*			Txt_UnitPrice;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*			Txt_LowestPrice;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*			Txt_EstimatedPrice;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*			Txt_Unit;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UEditableTextBox*	Input_Quantity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*			Btn_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*			Btn_Confirm;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*			Btn_Cancel;

	// 소비 아이템 전용


	

private:
	UPROPERTY()	FAuctionEntry Entry;
	bool bHasEntry = false;
	bool bIsEquipment = false;
	int32 BuyQuantity = 1;
};