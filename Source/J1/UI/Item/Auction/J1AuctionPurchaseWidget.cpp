// Copyright © 2026 Jerry. All rights reserved.

#include "J1AuctionPurchaseWidget.h"
#include "Data/J1ItemData.h"
#include "Item/J1ItemTemplate.h"
#include "Item/Fragments/J1ItemFragment_Equipable.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"

void UJ1AuctionPurchaseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_Confirm)	Btn_Confirm->OnClicked.AddDynamic(this, &UJ1AuctionPurchaseWidget::HandleConfirmClicked);
	if (Btn_Cancel)		Btn_Cancel->OnClicked.AddDynamic(this, &UJ1AuctionPurchaseWidget::HandleCancelClicked);
	if (Btn_Max)		Btn_Max->OnClicked.AddDynamic(this, &UJ1AuctionPurchaseWidget::HandleMaxClicked);
	if (Input_Quantity) Input_Quantity->OnTextChanged.AddDynamic(this, &UJ1AuctionPurchaseWidget::HandleQuantityTextChanged);
}

// 반드시 화면에 띄우기 전에 호출해서 채워야 한다.
void UJ1AuctionPurchaseWidget::SetEntry(const FAuctionEntry& InEntry)
{
	Entry = InEntry;
	bHasEntry = Entry.IsValid();
	bIsEquipment = false;

	if (Entry.ItemTemplateID > INDEX_NONE)
	{
		const UJ1ItemTemplate& ItemTemplate = UJ1ItemData::Get().FindItemTemplateByID(Entry.ItemTemplateID);
		bIsEquipment = (ItemTemplate.FindFragmentByClass<UJ1ItemFragment_Equipable>() != nullptr);

		if (Img_Icon && ItemTemplate.IconTexture)	Img_Icon->SetBrushFromTexture(ItemTemplate.IconTexture);
		if (Txt_Name)								Txt_Name->SetText(ItemTemplate.DisplayName);
	}

	// 소비 아이템 전용 위젯들 (최저가 / 구매 개수 입력 / MAX 버튼)
	const ESlateVisibility ConsumableOnlyVisibility = bIsEquipment ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;
	if (Txt_LowestPrice)
	{
		Txt_LowestPrice->SetVisibility(ConsumableOnlyVisibility);
		if (!bIsEquipment)							
			Txt_LowestPrice->SetText(FText::FromString(TEXT("조회 중...")));
	}
	if (Txt_Count)
	{
		Txt_Count->SetVisibility(ConsumableOnlyVisibility);
		if (!bIsEquipment)							
			Txt_Count->SetText(FText::Format(NSLOCTEXT("Auction", "SellerQuantity", "{0}개"), FText::AsNumber(Entry.Quantity)));	
	}
	if (Txt_UnitPrice)
	{
		Txt_UnitPrice->SetVisibility(ConsumableOnlyVisibility);
		if (!bIsEquipment)							
			Txt_UnitPrice->SetText(FText::AsNumber(Entry.GetPricePerUnit()));
	}
	if (Input_Quantity)		Input_Quantity->SetVisibility(ConsumableOnlyVisibility);
	if (Btn_Max)			Btn_Max->SetVisibility(ConsumableOnlyVisibility);
	
	// 수량 초기화: 장비는 항상 1, 소비 아이템은 1부터 시작 (사용자가 직접 늘리거나 MAX 사용)
	BuyQuantity = 1;
	if (!bIsEquipment && Input_Quantity)			Input_Quantity->SetText(FText::AsNumber(BuyQuantity));
	

	// 공통 사용 위젯
	if (Txt_Unit)
	{
		Txt_Unit->SetText(bIsEquipment ?
			FText::Format(NSLOCTEXT("Auction", "FixedQuantity", "{0}개"), FText::AsNumber(1))
			: FText::FromString(TEXT("개")));
	}

	if (Btn_Confirm)		Btn_Confirm->SetIsEnabled(bHasEntry);
	

	UpdateEstimatedPrice();

	if (!bIsEquipment)		RequestLowestPrice(Entry.ItemTemplateID);
	
}

// 동일 아이템 개당 최저가 서버 조회 응답이 도착했을 때 호출 (소비 아이템일 때만 의미 있음).
void UJ1AuctionPurchaseWidget::OnLowestPriceReceived(int64 LowestPrice)
{
	// 그 사이에 장비를 다시 골랐으면 무시
	if (bIsEquipment)	return; 

	if (Txt_LowestPrice)	Txt_LowestPrice->SetText(FText::Format(NSLOCTEXT("Auction", "FixedQuantity", "개당 최저가 {0}"), FText::AsNumber(LowestPrice)));
}

void UJ1AuctionPurchaseWidget::ClosePanel()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UJ1AuctionPurchaseWidget::RequestLowestPrice(int32 ItemTemplateID)
{
	// TODO: 실제 서버 요청으로 교체.
	// 예) AuctionSubsystem->RequestLowestPrice(ItemTemplateID,
	//         FOnLowestPriceReceived::CreateUObject(this, &UJ1AuctionPurchaseWidget::OnLowestPriceReceived));
}

void UJ1AuctionPurchaseWidget::UpdateEstimatedPrice()
{
	if (!Txt_EstimatedPrice)		return;
	
	const int64 EstimatedPrice = bIsEquipment ? Entry.TotalPrice : (Entry.GetPricePerUnit() * BuyQuantity);
	Txt_EstimatedPrice->SetText(FText::AsNumber(EstimatedPrice));
}

void UJ1AuctionPurchaseWidget::SetBuyQuantity(int32 NewQuantity)
{
	BuyQuantity = FMath::Clamp(NewQuantity, 1, FMath::Max(1, Entry.Quantity));
	UpdateEstimatedPrice();
}

void UJ1AuctionPurchaseWidget::HandleQuantityTextChanged(const FText& NewText)
{
	if (bIsEquipment)	return;

	const FString NewString = NewText.ToString();
	const int32 Requested = FCString::Atoi(*NewString);
	const int32 Clamped = FMath::Clamp(Requested, 1, FMath::Max(1, Entry.Quantity));

	if (Clamped != Requested)
	{
		// SetText가 OnTextChanged를 다시 트리거하지만, 그땐 Requested==Clamped라 재귀 X.
		if (Input_Quantity)		Input_Quantity->SetText(FText::AsNumber(Clamped));
		return;
	}

	SetBuyQuantity(Clamped);
}

void UJ1AuctionPurchaseWidget::HandleMaxClicked()
{
	if (bIsEquipment)	return;

	SetBuyQuantity(Entry.Quantity);
	if (Input_Quantity)
	{
		Input_Quantity->SetText(FText::AsNumber(BuyQuantity));
	}
}

void UJ1AuctionPurchaseWidget::HandleConfirmClicked()
{
	if (!bHasEntry)		return;

	const int32 FinalQuantity = bIsEquipment ? 1 : BuyQuantity;

	// TODO: 여기서 실제 구매 서버 호출을 하거나, OnPurchaseConfirmed 구독자가 처리하도록 위임.
	OnPurchaseConfirmed.Broadcast(Entry, FinalQuantity);

	ClosePanel();
}

void UJ1AuctionPurchaseWidget::HandleCancelClicked()
{
	ClosePanel();
}