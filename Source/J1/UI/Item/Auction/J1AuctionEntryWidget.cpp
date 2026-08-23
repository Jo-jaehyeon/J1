// Copyright © 2026 Jerry. All rights reserved.

#include "J1AuctionEntryWidget.h"
#include "Data/J1ItemData.h"
#include "Item/J1ItemTemplate.h"
#include "Item/Fragments/J1ItemFragment_Equipable.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UJ1AuctionEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_Receipt)
	{
		Btn_Receipt->OnClicked.AddDynamic(this, &UJ1AuctionEntryWidget::HandleReceiptClicked);
	}
}

FReply UJ1AuctionEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 내 판매 목록 모드에서는 행 클릭으로 아무것도 하지 않음
	if (bIsMyList)		return FReply::Unhandled(); 
	
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnEntryClicked.Broadcast(EntryIndex);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UJ1AuctionEntryWidget::InitEntry(int32 InEntryIndex, const FAuctionEntry& InEntry, bool bInIsMyListing)
{
	EntryIndex = InEntryIndex;
	Entry = InEntry;
	bIsMyList = bInIsMyListing;
	bClicked = false;

	bool bIsEquipment = false;

	if (Entry.ItemTemplateID > INDEX_NONE)
	{
		const UJ1ItemTemplate& ItemTemplate = UJ1ItemData::Get().FindItemTemplateByID(Entry.ItemTemplateID);
		bIsEquipment = (ItemTemplate.FindFragmentByClass<UJ1ItemFragment_Equipable>() != nullptr);

		if (Img_Icon && ItemTemplate.IconTexture)
		{
			Img_Icon->SetBrushFromTexture(ItemTemplate.IconTexture);
		}
		if (Txt_Name)
		{
			Txt_Name->SetText(ItemTemplate.DisplayName);
		}
	}

	if (Txt_Quantity)
	{
		Txt_Quantity->SetVisibility(bIsEquipment ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
		Txt_Quantity->SetText(FText::Format(NSLOCTEXT("Auction", "EntryQuantity", "{0}개"), FText::AsNumber(Entry.Quantity)));
	}
	if (Txt_TotalPrice)
	{
		Txt_TotalPrice->SetText(FText::AsNumber(Entry.TotalPrice));
	}
	if (Txt_PricePerUnit)
	{
		Txt_PricePerUnit->SetVisibility(bIsEquipment ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
		if (!bIsEquipment)
		{
			Txt_PricePerUnit->SetText(FText::AsNumber(Entry.GetPricePerUnit()));
		}
	}
	if (Txt_RemainingTime)
	{
		Txt_RemainingTime->SetText(FormatRemainingTime(Entry.ExpireAt));
	}

	// 수령 버튼은 내 판매 목록 모드 + 판매 완료일 때만 보인다. 그 외(경매장 엔트리 포함)엔 항상 숨김.
	const bool bShowClaimButton = bIsMyList && Entry.bIsSold;
	if (Btn_Receipt)
	{
		Btn_Receipt->SetVisibility(bShowClaimButton ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	// 경매장 엔트리(bIsMyListing=false)는 항상 "판매 안 됨" 기본 표현으로 통일.
	OnSoldStateChanged(bIsMyList && Entry.bIsSold);
	OnSelectionChanged(false);
}

FText UJ1AuctionEntryWidget::FormatRemainingTime(const FDateTime& ExpireAt)
{
	// 서버가 마감 시각을 KST(한국 시간) 기준으로 내려준다는 전제.
	// 클라이언트 PC의 로컬 시간대 설정과 무관하게 항상 같은 결과가 나오도록 UtcNow() + 9시간으로 KST를 직접 계산한다.
	static const FTimespan KstOffset = FTimespan::FromHours(9);
	const FDateTime NowKst = FDateTime::UtcNow() + KstOffset;

	const FTimespan Remaining = ExpireAt - NowKst;
	const int32 TotalSeconds = FMath::Max(0, static_cast<int32>(Remaining.GetTotalSeconds()));

	const int32 Hours = TotalSeconds / 3600;
	const int32 Minutes = (TotalSeconds % 3600) / 60;

	return FText::Format(NSLOCTEXT("Auction", "EntryRemainingTime", "{0}시간 {1}분"), FText::AsNumber(Hours), FText::AsNumber(Minutes));
}

void UJ1AuctionEntryWidget::SetSelected(bool bInClicked)
{
	// 내 판매 목록 모드에서는 선택 개념이 없음
	if (bIsMyList)		return; 
	
	bClicked = bInClicked;
	OnSelectionChanged(bInClicked);
}

void UJ1AuctionEntryWidget::HandleReceiptClicked()
{
	// 버튼이 숨겨져 있어야 정상이지만 한 번 더 확인
	if (!bIsMyList || !Entry.bIsSold)		return; 

	OnReceiptClicked.Broadcast(Entry);
}