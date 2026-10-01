// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Types/J1AuctionDefine.h"
#include "J1AuctionEntryWidget.generated.h"

class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEntryClicked, int32, EntryIndex);

/**
 * 경매장 목록의 엔트리 하나. 전체 목록(구매용)과 내 등록 물품 양쪽에서 공용으로 쓴다.
 * - 전체 목록 모드(bIsMyListing=false): 행 클릭으로 구매 대상 선택.
 * - 내 등록 물품 모드(bIsMyListing=true): 아직 안 팔린 내 매물 상태 확인용. 행 클릭은 무시.
 * 판매 대금/반환/구매 아이템 수령은 수령함 탭(UJ1AuctionReceiptEntryWidget)에서 처리한다.
 */
UCLASS()
class J1_API UJ1AuctionEntryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	// EntryIndex: 현재 페이지 안에서의 인덱스 (0~9). 부모가 클릭 판별에 사용 (구매 모드에서만 의미).
	UFUNCTION(BlueprintCallable, Category = "Auction")
	void InitEntry(int32 InEntryIndex, const FAuctionEntry& InEntry, bool bInIsMyListing);

	// 구매 모드에서만 의미 있음. 내 등록 물품 모드에서는 호출해도 아무 효과 없음.
	UFUNCTION(BlueprintCallable, Category = "Auction")
	void SetSelected(bool bInClicked);

	UFUNCTION(BlueprintPure, Category = "Auction")
	const FAuctionEntry& GetEntry() const { return Entry; }

	FText FormatRemainingTime(const FString& ExpireAtString);

protected:
	// 구매 모드에서 선택됐을 때의 시각 표현(테두리 강조 등)은 디자이너가 블루프린트에서 구현
	UFUNCTION(BlueprintImplementableEvent, Category = "Auction")
	void OnSelectionChanged(bool bInClicked);

public:
	// ═════════════════════
	//		 DELEGATE
	// ═════════════════════
	UPROPERTY(BlueprintAssignable, Category = "Auction")	FOnEntryClicked	OnEntryClicked;

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

private:
	int32	EntryIndex = INDEX_NONE;
	bool	bClicked = false;
	bool	bIsMyList = false;

	UPROPERTY()		FAuctionEntry Entry;
};
