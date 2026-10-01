// Copyright © 2026 Jerry. All rights reserved.

#include "J1AuctionReceiptEntryWidget.h"
#include "Data/J1ItemData.h"
#include "Item/J1ItemTemplate.h"
#include "Item/Fragments/J1ItemFragment_Equipable.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UJ1AuctionReceiptEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_Receipt)		Btn_Receipt->OnClicked.AddDynamic(this, &UJ1AuctionReceiptEntryWidget::HandleReceiptClicked);
}

void UJ1AuctionReceiptEntryWidget::InitReceiptEntry(const FAuctionReceiptEntry& InReceiptEntry)
{
	ReceiptEntry = InReceiptEntry;
	SetReceiptPending(false);

	const bool bIsGold = ReceiptEntry.IsGoldReceipt();
	const ESlateVisibility GoldOnly = bIsGold ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	const ESlateVisibility ItemOnly = bIsGold ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;

	if (Txt_ReceiptType)
	{
		switch (ReceiptEntry.ReceiptType)
		{
		case EAuctionReceiptType::SoldProceeds:	Txt_ReceiptType->SetText(NSLOCTEXT("Auction", "ReceiptSold", "판매 대금"));		break;
		case EAuctionReceiptType::ReturnedItem:	Txt_ReceiptType->SetText(NSLOCTEXT("Auction", "ReceiptReturned", "기간 만료"));	break;
		case EAuctionReceiptType::PurchasedItem:	Txt_ReceiptType->SetText(NSLOCTEXT("Auction", "ReceiptPurchased", "구매 완료"));	break;
		}
	}

	// 골드 수령
	if (Txt_TotalPrice)
	{
		Txt_TotalPrice->SetVisibility(GoldOnly);
		if (bIsGold)	Txt_TotalPrice->SetText(FText::AsNumber(ReceiptEntry.GoldAmount));
	}

	// 아이템 수령 (반환 / 구매)
	bool bIsEquipment = false;
	if (!bIsGold && ReceiptEntry.ItemTemplateID > INDEX_NONE)
	{
		const UJ1ItemTemplate& ItemTemplate = UJ1ItemData::Get().FindItemTemplateByID(ReceiptEntry.ItemTemplateID);
		bIsEquipment = (ItemTemplate.FindFragmentByClass<UJ1ItemFragment_Equipable>() != nullptr);

		if (Img_Icon && ItemTemplate.IconTexture)	Img_Icon->SetBrushFromTexture(ItemTemplate.IconTexture);
		if (Txt_Name)								Txt_Name->SetText(ItemTemplate.DisplayName);
	}

	if (Img_Icon)		Img_Icon->SetVisibility(ItemOnly);
	if (Txt_Name)		Txt_Name->SetVisibility(ItemOnly);
	if (Txt_Quantity)
	{
		// 장비는 항상 1개라 개수 표시 생략 (경매장 엔트리와 동일 규칙)
		Txt_Quantity->SetVisibility((bIsGold || bIsEquipment) ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
		Txt_Quantity->SetText(FText::Format(NSLOCTEXT("Auction", "ReceiptQuantity", "{0}개"), FText::AsNumber(ReceiptEntry.Quantity)));
	}

	OnReceiptTypeChanged(ReceiptEntry.ReceiptType);
}

void UJ1AuctionReceiptEntryWidget::SetReceiptPending(bool bInPending)
{
	bPending = bInPending;
	if (Btn_Receipt)		Btn_Receipt->SetIsEnabled(!bPending);
}

void UJ1AuctionReceiptEntryWidget::HandleReceiptClicked()
{
	if (bPending || !ReceiptEntry.IsValid())		return;

	SetReceiptPending(true);
	OnReceiptClicked.Broadcast(ReceiptEntry);
}
