// Copyright © 2026 Jerry. All rights reserved.

#include "J1AuctionRegisterWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Manager/ActorComponent/J1InventoryManager.h"
#include "Item/J1ItemInstance.h"
#include "Item/J1ItemTemplate.h"
#include "Item/Fragments/J1ItemFragment_Equipable_Utility.h"
#include "UI/Item/Inventory/J1InventoryWidget.h"

void UJ1AuctionRegisterWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_Register)	Btn_Register->OnClicked.AddDynamic(this, &UJ1AuctionRegisterWidget::HandleRegisterClicked);
	if (Btn_Cancel)		Btn_Cancel->OnClicked.AddDynamic(this, &UJ1AuctionRegisterWidget::HandleCancelClicked);
	if (Check_Duration)
	{
		Check_Duration->OnCheckStateChanged.AddDynamic(this, &UJ1AuctionRegisterWidget::HandleDurationToggleChanged);
		Check_Duration->SetIsChecked(false);		// 체크 해제 상태 = 24시간을 기본값으로.
	}
	if (Input_Quantity) Input_Quantity->OnTextChanged.AddDynamic(this, &UJ1AuctionRegisterWidget::HandleQuantityChanged);
	if (Input_Price)	Input_Price->OnTextChanged.AddDynamic(this, &UJ1AuctionRegisterWidget::HandlePriceChanged);

	SelectedDurationHours = "24";
}

void UJ1AuctionRegisterWidget::NativeDestruct()
{
	UnbindFromInventory();
	Super::NativeDestruct();
}

void UJ1AuctionRegisterWidget::BindToInventory(UUserWidget* InInventoryWidget)
{
	UnbindFromInventory();

	if (UJ1InventoryWidget* InventoryWidget = Cast<UJ1InventoryWidget>(InInventoryWidget))
	{
		InventoryWidget->OnItemSelected.AddDynamic(this, &UJ1AuctionRegisterWidget::HandleItemSelected);
		BoundInventoryWidget = InventoryWidget;
	}
}

void UJ1AuctionRegisterWidget::UnbindFromInventory()
{
	if (UJ1InventoryWidget* InventoryWidget = Cast<UJ1InventoryWidget>(BoundInventoryWidget.Get()))
	{
		InventoryWidget->OnItemSelected.RemoveDynamic(this, &UJ1AuctionRegisterWidget::HandleItemSelected);
	}
	BoundInventoryWidget = nullptr;
}

void UJ1AuctionRegisterWidget::ClosePanel()
{
	SetVisibility(ESlateVisibility::Collapsed);

	if (Img_Icon)			Img_Icon->SetBrushFromTexture(nullptr);
	if (Txt_Name)			Txt_Name->SetText(FText::FromString(TEXT("")));
	if (Txt_LowestPrice)	Txt_LowestPrice->SetText(FText::FromString(TEXT("조회 중...")));
	if (Input_Quantity)		Input_Quantity->SetText(FText::FromString(TEXT("")));
	if (Input_Price)		Input_Price->SetText(FText::FromString(TEXT("")));
	if (Txt_TotalPrice)		Txt_TotalPrice->SetText(FText::AsNumber(0));

	// 다음에 열었을 때 이전 선택이 남아서 화면과 다른 아이템이 등록되지 않도록 초기화
	SelectedInventory = nullptr;
	SelectedSlotIndex = INDEX_NONE;
	SelectedItemMaxQuantity = 1;

	// 등록 성공 후 닫힌 경우 버튼이 비활성 상태로 남아있으므로 다시 켜준다.
	if (Btn_Register)		Btn_Register->SetIsEnabled(true);
	if (Btn_Cancel)			Btn_Cancel->SetIsEnabled(true);
}

void UJ1AuctionRegisterWidget::HandleRegisterResult(bool bSuccess)
{
	// 제출한 적 없는데 결과가 온 경우 - 방어적으로 무시
	if (!bIsRegist)		return;

	bIsRegist = false;
	OnSubmittingStateChanged(false);

	if (bSuccess)
	{
		// 성공 : 실제로 인벤토리에서 아이템을 빼고 창을 닫는다.
		if (SelectedInventory.IsValid() && SelectedSlotIndex != INDEX_NONE)
		{
			const int32 Quantity = bSelectedItemIsEquipment
				? 1	: (Input_Quantity ? FMath::Clamp(FCString::Atoi(*Input_Quantity->GetText().ToString()), 1, SelectedItemMaxQuantity) : 1);
			SelectedInventory->RemoveItemAt(SelectedSlotIndex, Quantity);
		}

		// 경매장 메인 위젯의 자식이므로 RemoveFromParent가 아니라 Collapsed로 숨긴다.
		ClosePanel();
	}
	else
	{
		// 실패 : 등록 버튼만 다시 활성화해서 재시도
		if (Btn_Register)	Btn_Register->SetIsEnabled(true);
		if (Btn_Cancel)		Btn_Cancel->SetIsEnabled(true);
		OnRegisterFailed();
	}
}

void UJ1AuctionRegisterWidget::OnLowestPriceReceived(int64 LowestPrice)
{
	// 그 사이에 장비를 다시 골랐으면 무시
	if (bSelectedItemIsEquipment)	return; 

	if (Txt_LowestPrice)	Txt_LowestPrice->SetText(FText::Format(NSLOCTEXT("Auction", "FixedQuantity", "개당 최저가 {0}"), FText::AsNumber(LowestPrice)));
}

void UJ1AuctionRegisterWidget::HandleItemSelected(UJ1InventoryManager* InInventory, int32 SlotIndex)
{
	// 등록 패널이 닫혀있거나 제출 중일 땐 인벤토리 클릭을 무시
	if (!InInventory || bIsRegist || !IsVisible())	return;

	const FJ1InventorySlot IS = InInventory->GetSlot(SlotIndex);
	if (IS.IsEmpty())	return;

	const UJ1ItemTemplate* ItemTemplate = IS.ItemInstance->GetItemTemplate();
	if (!ItemTemplate)	return;
	
	SelectedInventory = InInventory;
	SelectedSlotIndex = SlotIndex;
	SelectedItemMaxQuantity = IS.StackCount;

	// 장비인지 소비 아이템인지 판정 (UseItem()과 동일한 기준: Equipable Fragment 존재 여부)
	bSelectedItemIsEquipment = (ItemTemplate->FindFragmentByClass<UJ1ItemFragment_Equipable_Utility>() == nullptr);

	if (Img_Icon && ItemTemplate->IconTexture)
	{
		Img_Icon->SetBrushFromTexture(ItemTemplate->IconTexture);
	}
	if (Txt_Name)
	{
		Txt_Name->SetText(ItemTemplate->DisplayName);
	}

	// 소비 아이템일 때만 개당 최저가 / 판매 개수 필드를 보여주고 실제 레이아웃을 차지하게 함.
	const ESlateVisibility ConsumableOnlyVisibility = bSelectedItemIsEquipment ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;
	if (Txt_LowestPrice)
	{
		Txt_LowestPrice->SetVisibility(ConsumableOnlyVisibility);
		if (!bSelectedItemIsEquipment)
		{
			Txt_LowestPrice->SetText(FText::FromString(TEXT("조회 중...")));
		}
	}
	if (Input_Quantity)
	{
		Input_Quantity->SetVisibility(ConsumableOnlyVisibility);
		if (!bSelectedItemIsEquipment)
		{
			Input_Quantity->SetText(FText::AsNumber(1));
		}
	}
	if (Txt_TotalPrice)
	{
		Txt_TotalPrice->SetVisibility(ConsumableOnlyVisibility);
	}

	UpdateTotalPrice();

	if (!bSelectedItemIsEquipment)
	{
		RequestLowestPrice(IS.ItemInstance->GetItemTemplateID());
	}
}

void UJ1AuctionRegisterWidget::HandleRegisterClicked()
{
	// 중복 클릭 방지
	if (bIsRegist)		return;

	// 아직 인벤토리에서 아이템을 고르지 않음
	if (!SelectedInventory.IsValid() || SelectedSlotIndex == INDEX_NONE)
		return; 

	const int64 PricePerUnit = Input_Price ? FCString::Atoi64(*Input_Price->GetText().ToString()) : 0;
	if (PricePerUnit <= 0)
	{
		return; // TODO: 가격 미입력/0원 안내 UI 처리
	}

	// 보유 수량(SelectedItemMaxQuantity)을 넘지 않게 clamp.
	int32 Quantity = 1;
	if (!bSelectedItemIsEquipment)
	{
		const int32 RequestedQuantity = Input_Quantity ? FCString::Atoi(*Input_Quantity->GetText().ToString()) : 1;
		Quantity = FMath::Clamp(RequestedQuantity, 1, SelectedItemMaxQuantity);
	}

	bIsRegist = true;
	if (Btn_Register)	Btn_Register->SetIsEnabled(false);
	if (Btn_Cancel)		Btn_Cancel->SetIsEnabled(false);
	
	OnSubmittingStateChanged(true);
	OnRegisterSubmitted.Broadcast(SelectedInventory.Get(), SelectedSlotIndex, PricePerUnit, Quantity, SelectedDurationHours);
}

void UJ1AuctionRegisterWidget::HandleCancelClicked()
{
	// 방어적 체크 - 버튼이 비활성화돼 있어야 정상이지만 한 번 더 확인
	if (bIsRegist)		return;

	ClosePanel();
}

void UJ1AuctionRegisterWidget::HandleDurationToggleChanged(bool bIsChecked)
{
	SelectedDurationHours = bIsChecked ? "48" : "24";
}

void UJ1AuctionRegisterWidget::HandleQuantityChanged(const FText& NewText)
{
	if (bSelectedItemIsEquipment)		return;
	
	const FString NewString = NewText.ToString();
	const int32 Requested = FCString::Atoi(*NewString);
	const int32 Clamped = FMath::Clamp(Requested, 1, FMath::Max(1, SelectedItemMaxQuantity));

	if (Clamped != Requested)
	{
		// SetText가 OnTextChanged를 다시 트리거하지만, 그땐 Requested==Clamped라  재귀 X.
		if (Input_Quantity)		Input_Quantity->SetText(FText::AsNumber(Clamped));
		return;
	}

	UpdateTotalPrice();
}

void UJ1AuctionRegisterWidget::HandlePriceChanged(const FText& NewText)
{
	UpdateTotalPrice();
}

void UJ1AuctionRegisterWidget::RequestLowestPrice(int32 ItemTemplateID)
{
	// TODO: 실제 서버 요청으로 교체.
	// 예) AuctionSubsystem->RequestLowestPrice(ItemTemplateID,
	//         FOnLowestPriceReceived::CreateUObject(this, &UJ1AuctionRegisterWidget::OnLowestPriceReceived));
}

void UJ1AuctionRegisterWidget::UpdateTotalPrice()
{
	// 장비는 TotalPriceText 자체를 Collapsed 처리했으니 계산도 필요 없음
	if (!Txt_TotalPrice || bSelectedItemIsEquipment)		return; 
	
	const int32 Quantity = Input_Quantity ? FCString::Atoi(*Input_Quantity->GetText().ToString()) : 0;
	const int64 PricePerUnit = Input_Price ? FCString::Atoi64(*Input_Price->GetText().ToString()) : 0;

	const int64 Total = static_cast<int64>(FMath::Max(0, Quantity)) * FMath::Max<int64>(0, PricePerUnit);
	Txt_TotalPrice->SetText(FText::AsNumber(Total));
}