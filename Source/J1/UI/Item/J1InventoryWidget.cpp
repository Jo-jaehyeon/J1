// Copyright © 2026 Jerry. All rights reserved.

#include "J1InventoryWidget.h"
#include "J1InventorySlotWidget.h"
#include "Manager/ActorComponent/J1InventoryManager.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UJ1InventoryWidget::InitInventory(UJ1InventoryManager* InInventory)
{
	if (Inventory.IsValid())
	{
		Inventory->OnInventoryChanged.RemoveDynamic(this, &UJ1InventoryWidget::HandleInventoryChanged);
		Inventory->OnSlotChanged.RemoveDynamic(this, &UJ1InventoryWidget::HandleSlotChanged);
	}

	Inventory = InInventory;

	if (Inventory.IsValid())
	{
		Inventory->OnInventoryChanged.AddDynamic(this, &UJ1InventoryWidget::HandleInventoryChanged);
		Inventory->OnSlotChanged.AddDynamic(this, &UJ1InventoryWidget::HandleSlotChanged);
	}

	RebuildGrid();
}

void UJ1InventoryWidget::RebuildGrid()
{
	if (!UGP_ItemSlots || !Inventory.IsValid() || !SlotWidgetClass)
	{
		return;
	}

	UGP_ItemSlots->ClearChildren();
	SlotWidgets.Reset();

	const int32 Columns = Inventory->Columns;
	const int32 SlotCount = Inventory->GetSlotCount();

	for (int32 Index = 0; Index < SlotCount; ++Index)
	{
		UJ1InventorySlotWidget* SlotWidget = CreateWidget<UJ1InventorySlotWidget>(this, SlotWidgetClass);
		if (!SlotWidget) continue;

		SlotWidget->InitSlot(Inventory.Get(), Index);

		const int32 Row = Index / Columns;
		const int32 Col = Index % Columns;

		UUniformGridSlot* GridSlot = UGP_ItemSlots->AddChildToUniformGrid(SlotWidget, Row, Col);
		if (GridSlot)
		{
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}

		SlotWidgets.Add(SlotWidget);
	}
}

void UJ1InventoryWidget::HandleInventoryChanged()
{
	// 슬롯 개수(Columns*Rows)가 바뀐 경우를 포함해 전체를 다시 그림.
	// 매번 풀 리빌드가 부담이면, 슬롯 개수가 그대로일 때는 각 SlotWidget->RefreshVisuals()만
	// 순회 호출하도록 최적화 가능.
	if (Inventory.IsValid() && SlotWidgets.Num() != Inventory->GetSlotCount())
	{
		RebuildGrid();
	}
	else
	{
		for (UJ1InventorySlotWidget* SlotWidget : SlotWidgets)
		{
			if (SlotWidget)
			{
				SlotWidget->RefreshVisuals();
			}
		}
	}
}

void UJ1InventoryWidget::HandleSlotChanged(int32 SlotIndex)
{
	if (SlotWidgets.IsValidIndex(SlotIndex) && SlotWidgets[SlotIndex])
	{
		SlotWidgets[SlotIndex]->RefreshVisuals();
	}
}