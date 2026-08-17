// Copyright © 2026 Jerry. All rights reserved.

#include "J1InventorySlotWidget.h"
#include "Manager/ActorComponent/J1InventoryManager.h"
#include "UI/Item/J1DragDrop.h"
#include "UI/Item/J1ItemDragWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Item/J1ItemDefinition.h"

void UJ1InventorySlotWidget::InitSlot(UJ1InventoryManager* InInventory, int32 InSlotIndex)
{
	Inventory = InInventory;
	SlotIndex = InSlotIndex;
	RefreshVisuals();
}

void UJ1InventorySlotWidget::RefreshVisuals()
{
	if (!Inventory.IsValid()) return;

	const FInventorySlot IS = Inventory->GetSlot(SlotIndex);
	const bool bHasItem = !IS.IsEmpty();

	if (Img_Icon)
	{
		Img_Icon->SetVisibility(bHasItem ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		if (bHasItem)
		{
			if (const UJ1ItemDefinition* Def = IS.Item.ItemDefinition.LoadSynchronous())
			{
				if (UTexture2D* Texture = Def->Icon.LoadSynchronous())
				{
					Img_Icon->SetBrushFromTexture(Texture);
				}
			}
		}
	}

	if (Txt_StackCount)
	{
		const bool bShowCount = bHasItem && IS.Item.StackCount > 1;
		Txt_StackCount->SetVisibility(bShowCount ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		Txt_StackCount->SetText(FText::AsNumber(IS.Item.StackCount));
	}
}

FReply UJ1InventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (Inventory.IsValid())
		{
			Inventory->UseItem(SlotIndex);
		}
		return FReply::Handled();
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return FReply::Unhandled();
}

void UJ1InventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (!Inventory.IsValid() || Inventory->GetSlot(SlotIndex).IsEmpty())
	{
		return;
	}

	UJ1DragDrop* Op = NewObject<UJ1DragDrop>(this);
	Op->SourceSlotIndex = SlotIndex;
	Op->SourceInventory = Inventory.Get();
	Op->DefaultDragVisual = this; // 필요하면 아이콘만 보여주는 전용 드래그 비주얼 위젯으로 교체
	Op->Pivot = EDragPivot::MouseDown;

	// 주의: this(그리드에 배치된 실제 슬롯 위젯)를 DefaultDragVisual로 재사용하면
	// 위젯 트리에서 뜯겨나가 그리드에서 사라지고, 드롭 이벤트도 항상 이 위젯 자신이
	// 가로채게 된다. 반드시 별도 인스턴스를 새로 만들어서 사용해야 한다.
	const TSubclassOf<UJ1ItemDragWidget> VisualClass =	(ItemDragWidgetClass) ? ItemDragWidgetClass : TSubclassOf<UJ1ItemDragWidget>(UJ1ItemDragWidget::StaticClass());

	if (UJ1ItemDragWidget* Visual = CreateWidget<UJ1ItemDragWidget>(this, VisualClass))
	{
		if (Img_Icon)
		{
			if (UTexture2D* Texture = Cast<UTexture2D>(Img_Icon->Brush.GetResourceObject()))
			{
				Visual->SetIconTexture(Texture);
			}
		}
		Op->DefaultDragVisual = Visual;
	}


	OutOperation = Op;
}

bool UJ1InventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop called on SlotIndex=%d"), SlotIndex);

	UJ1DragDrop* Op = Cast<UJ1DragDrop>(InOperation);
	if (!Op || !Op->SourceInventory.IsValid() || !Inventory.IsValid())
	{
		return false;
	}

	// 같은 인벤토리 내 이동만 처리. 상자<->인벤토리 같은 이동이 필요하면
	// InventoryComponent에 TransferItem(TargetInventory, FromIndex, ToIndex) 추가해서 확장.
	if (Op->SourceInventory.Get() != Inventory.Get())
	{
		return false;
	}

	// MoveItem이 내부적으로 no-op(같은 슬롯 등)이어도 이 드롭은 우리 인벤토리 영역 안에서
	// "처리된" 것으로 간주해야 한다. false를 반환하면 UMG가 이 드래그를 실패로 보고
	// NativeOnDragCancelled를 추가 호출해서 아이템을 버리게 되므로, 여기서는 항상 true.
	Inventory->MoveItem(Op->SourceSlotIndex, SlotIndex);

	return true;
}

void UJ1InventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Warning, TEXT("NativeOnDragCancelled called, source SlotIndex=%d"), SlotIndex);

	// 이 함수는 드롭 대상이 NativeOnDrop을 처리하지 못했을 때(=인벤토리 슬롯 바깥에서 놓았을 때)
	// 드래그를 시작한 위젯 쪽에서 호출됨 -> 여기서 "버리기"로 처리
	UJ1DragDrop* Op = Cast<UJ1DragDrop>(InOperation);
	if (Op && Op->SourceInventory.IsValid())
	{
		Op->SourceInventory->DropItem(Op->SourceSlotIndex);
	}
}