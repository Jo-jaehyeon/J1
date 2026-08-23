// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Types/J1AuctionDefine.h"
#include "J1AuctionEntryWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEntryClicked, int32, EntryIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiptClicked, FAuctionEntry, Entry);

/**
 * 경매장 목록의 엔트리 하나. 전체 목록(구매용)과 내 판매 목록 양쪽에서 공용으로 쓴다.
 * - 전체 목록 모드(bIsMyListing=false): 행 클릭으로 구매 대상 선택, 수령 버튼은 항상 숨김.
 * - 내 판매 목록 모드(bIsMyListing=true): 행 클릭은 무시, 판매 완료(Entry.bIsSold)일 때만
 *   수령 버튼이 보이고, 텍스트 색도 등록 중/판매 완료에 따라 달라진다.
 */
UCLASS()
class J1_API UJ1AuctionEntryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	// EntryIndex: 현재 페이지 안에서의 인덱스 (0~9). 부모가 클릭 판별에 사용 (구매 모드에서만 의미).
	UFUNCTION(BlueprintCallable, Category = "Auction")
	void InitEntry(int32 InEntryIndex, const FAuctionEntry& InEntry, bool bInIsMyListing);

	// 구매 모드에서만 의미 있음. 내 판매 목록 모드에서는 호출해도 아무 효과 없음.
	UFUNCTION(BlueprintCallable, Category = "Auction")
	void SetSelected(bool bInClicked);

	UFUNCTION(BlueprintPure, Category = "Auction")
	const FAuctionEntry& GetEntry() const { return Entry; }

	FText FormatRemainingTime(const FDateTime& ExpireAt);

protected:
	// 구매 모드에서 선택됐을 때의 시각 표현(테두리 강조 등)은 디자이너가 블루프린트에서 구현
	UFUNCTION(BlueprintImplementableEvent, Category = "Auction")
	void OnSelectionChanged(bool bInClicked);

	// 내 판매 목록 모드에서 판매 완료 여부에 따른 텍스트 색 등 표현은 디자이너가 블루프린트에서 구현.
	// bIsMyListing==false일 때는 항상 false로 호출된다 (경매장 엔트리와 동일한 기본 표현).
	UFUNCTION(BlueprintImplementableEvent, Category = "Auction")
	void OnSoldStateChanged(bool bIsSold);

private:
	UFUNCTION()		void HandleReceiptClicked();

public:
	// ═════════════════════
	//		 DELEGATE
	// ═════════════════════
	UPROPERTY(BlueprintAssignable, Category = "Auction")	FOnEntryClicked	OnEntryClicked;
	UPROPERTY(BlueprintAssignable, Category = "Auction")	FOnReceiptClicked OnReceiptClicked;

protected:
	// ════════════════════════════════════
	//              UMG 바인딩
	// ════════════════════════════════════
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UImage*			Img_Icon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*		Txt_Quantity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*		Txt_Name;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*		Txt_TotalPrice;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*		Txt_PricePerUnit;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*		Txt_RemainingTime;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		Btn_Receipt;

private:
	int32	EntryIndex = INDEX_NONE;
	bool	bClicked = false;
	bool	bIsMyList = false;

	UPROPERTY()		FAuctionEntry Entry;
};