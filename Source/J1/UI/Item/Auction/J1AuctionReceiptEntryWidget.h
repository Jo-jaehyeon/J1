// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Types/J1AuctionDefine.h"
#include "J1AuctionReceiptEntryWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiptClicked, FAuctionReceiptEntry, ReceiptEntry);

/**
 * 수령함 탭의 엔트리 하나. (DB auction_receipts 한 행)
 * - SoldProceeds  : 골드 금액만 표시 (아이콘/이름/개수 숨김)
 * - ReturnedItem  : 아이템 아이콘/이름/개수 표시
 * - PurchasedItem : 아이템 아이콘/이름/개수 표시
 * 수령 버튼은 항상 보인다. 실제 지급은 서버 성공 응답 후에만 이뤄진다 (확정 갱신).
 */
UCLASS()
class J1_API UJ1AuctionReceiptEntryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Auction")
	void InitReceiptEntry(const FAuctionReceiptEntry& InReceiptEntry);

	// 수령 요청을 보낸 뒤 응답이 올 때까지 버튼을 잠근다 (중복 수령 방지).
	UFUNCTION(BlueprintCallable, Category = "Auction")
	void SetReceiptPending(bool bInPending);

	UFUNCTION(BlueprintPure, Category = "Auction")
	const FAuctionReceiptEntry& GetReceiptEntry() const { return ReceiptEntry; }

protected:
	// 수령 종류(판매 대금/반환/구매)에 따른 색상·라벨 등 표현은 디자이너가 블루프린트에서 구현
	UFUNCTION(BlueprintImplementableEvent, Category = "Auction")
	void OnReceiptTypeChanged(EAuctionReceiptType InReceiptType);

private:
	UFUNCTION()		void HandleReceiptClicked();

public:
	// ═════════════════════
	//		 DELEGATE
	// ═════════════════════
	UPROPERTY(BlueprintAssignable, Category = "Auction")	FOnReceiptClicked OnReceiptClicked;

protected:
	// ════════════════════════════════════
	//              UMG 바인딩
	// ════════════════════════════════════
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UImage*			Img_Icon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*		Txt_Name;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*		Txt_Quantity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*		Txt_TotalPrice;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*		Txt_ReceiptType;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		Btn_Receipt;

private:
	bool	bPending = false;

	UPROPERTY()		FAuctionReceiptEntry ReceiptEntry;
};
