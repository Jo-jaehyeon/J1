// Copyright © 2026 Jerry. All rights reserved.

#include "J1AuctionEntryWidget.h"
#include "Data/J1ItemData.h"
#include "Item/J1ItemTemplate.h"
#include "Item/Fragments/J1ItemFragment_Equipable.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

FReply UJ1AuctionEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 내 등록 물품 모드에서는 행 클릭으로 아무것도 하지 않음
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
		Txt_Quantity->SetVisibility(bIsEquipment ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
		Txt_Quantity->SetText(FText::Format(NSLOCTEXT("Auction", "EntryQuantity", "{0}개"), FText::AsNumber(Entry.Quantity)));
	}
	if (Txt_TotalPrice)
	{
		Txt_TotalPrice->SetText(FText::AsNumber(Entry.GetTotalPrice()));
	}
	if (Txt_PricePerUnit)
	{
		Txt_PricePerUnit->SetVisibility(bIsEquipment ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
		if (!bIsEquipment)
		{
			Txt_PricePerUnit->SetText(FText::AsNumber(Entry.UnitPrice));
		}
	}
	if (Txt_RemainingTime)
	{
		Txt_RemainingTime->SetText(FormatRemainingTime(Entry.ExpireAt));
	}

	OnSelectionChanged(false);
}

FText UJ1AuctionEntryWidget::FormatRemainingTime(const FString& ExpireAtString)
{
	// ExpireAtString은 MySQL DATETIME("YYYY-MM-DD HH:MM:SS")이 std::string -> FString으로 넘어온 값.
	FString Iso8601String = ExpireAtString;
	Iso8601String.ReplaceInline(TEXT(" "), TEXT("T"));

	FDateTime ExpireAt;
	if (!FDateTime::ParseIso8601(*Iso8601String, ExpireAt))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to parse ExpireAt string: %s"), *ExpireAtString);
		return FText::Format(NSLOCTEXT("Auction", "EntryRemainingTime", "{0}시간 {1}분"), FText::AsNumber(0), FText::AsNumber(0));
	}

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
	// 내 등록 물품 모드에서는 선택 개념이 없음
	if (bIsMyList)		return;

	bClicked = bInClicked;
	OnSelectionChanged(bInClicked);
}
