// Copyright © 2026 Jerry. All rights reserved.

#include "J1InventoryManager.h"
#include "Item/J1ItemInstance.h"
#include "Item/J1ItemTemplate.h"

UJ1InventoryManager::UJ1InventoryManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UJ1InventoryManager::BeginPlay()
{
	Super::BeginPlay();
	ResizeInventory(Columns, Rows);
}

void UJ1InventoryManager::ResizeInventory(int32 NewColumns, int32 NewRows)
{
	Columns = FMath::Max(1, NewColumns);
	Rows = FMath::Max(1, NewRows);
	const int32 NewCount = Columns * Rows;

	if (NewCount > Slots.Num())
	{
		Slots.AddDefaulted(NewCount - Slots.Num());
	}
	else if (NewCount < Slots.Num())
	{
		// 주의: 줄어드는 슬롯에 아이템이 있으면 유실됨.
		// 실제 서비스에서는 줄이기 전에 남는 아이템을 앞쪽 빈 슬롯으로 먼저 재배치할 것.
		Slots.SetNum(NewCount);
	}

	OnInventoryChanged.Broadcast();
}

FJ1InventorySlot UJ1InventoryManager::GetSlot(int32 Index) const
{
	return Slots.IsValidIndex(Index) ? Slots[Index] : FJ1InventorySlot();
}

int32 UJ1InventoryManager::AddItem(UJ1ItemInstance* NewItem, int32 Count)
{
	if (!NewItem || Count <= 0)
	{
		return 0;
	}

	int32 Remaining = Count;
	const int32 MaxStack = GetMaxStackCount(NewItem);

	// 1) 기존 스택에 우선 병합
	if (MaxStack > 1)
	{
		for (FJ1InventorySlot& Slot : Slots)
		{
			if (Remaining <= 0) break;
			if (Slot.IsEmpty()) continue;
			if (!Slot.ItemInstance->CanStackWith(NewItem)) continue;

			const int32 Space = MaxStack - Slot.StackCount;
			if (Space <= 0) continue;

			const int32 ToMove = FMath::Min(Space, Remaining);
			Slot.StackCount += ToMove;
			Remaining -= ToMove;
		}
	}

	// 2) 남은 수량은 빈 슬롯에 새로 배치 (여러 슬롯에 나눠 담김)
	for (int32 i = 0; i < Slots.Num() && Remaining > 0; ++i)
	{
		if (!Slots[i].IsEmpty()) continue;

		Slots[i].ItemInstance = NewItem;
		Slots[i].StackCount = FMath::Min(MaxStack, Remaining);
		Remaining -= Slots[i].StackCount;
	}

	OnInventoryChanged.Broadcast();
	return Remaining; // 0보다 크면 인벤토리가 가득 찬 것
}

bool UJ1InventoryManager::RemoveItemAt(int32 SlotIndex, int32 Count)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty())
	{
		return false;
	}

	FJ1InventorySlot& Slot = Slots[SlotIndex];
	Slot.StackCount -= Count;
	if (Slot.StackCount <= 0)
	{
		Slot.Reset();
	}

	OnSlotChanged.Broadcast(SlotIndex);
	OnInventoryChanged.Broadcast();
	return true;
}

bool UJ1InventoryManager::MoveItem(int32 FromIndex, int32 ToIndex)
{
	if (FromIndex == ToIndex) return false;
	if (!Slots.IsValidIndex(FromIndex) || !Slots.IsValidIndex(ToIndex)) return false;
	if (Slots[FromIndex].IsEmpty()) return false;

	FJ1InventorySlot& From = Slots[FromIndex];
	FJ1InventorySlot& To = Slots[ToIndex];

	if (To.IsEmpty())
	{
		To.ItemInstance = From.ItemInstance;
		To.StackCount = From.StackCount;
		From.Reset();
	}
	else if (To.ItemInstance->CanStackWith(From.ItemInstance))
	{
		const int32 MaxStack = GetMaxStackCount(To.ItemInstance);
		const int32 Space = MaxStack - To.StackCount;
		const int32 ToMove = FMath::Min(Space, From.StackCount);

		To.StackCount += ToMove;
		From.StackCount -= ToMove;
		if (From.StackCount <= 0)
		{
			From.Reset();
		}
	}
	else
	{
		// 다른 아이템, 혹은 스택 불가(장비) -> 위치 스왑
		Swap(From.ItemInstance, To.ItemInstance);
		Swap(From.StackCount, To.StackCount);
	}

	OnSlotChanged.Broadcast(FromIndex);
	OnSlotChanged.Broadcast(ToIndex);

	return true;
}

bool UJ1InventoryManager::DropItem(int32 SlotIndex, int32 DropCount)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty())
	{
		return false;
	}

	const int32 CountToDrop = (DropCount < 0)
		? Slots[SlotIndex].StackCount
		: FMath::Min(DropCount, Slots[SlotIndex].StackCount);

	// TODO: 여기서 실제 월드 픽업 액터를 스폰.
	// 예) GetWorld()->SpawnActor<AJ1ItemPickup>(...)->InitFromItemInstance(Slots[SlotIndex].ItemInstance, CountToDrop);

	return RemoveItemAt(SlotIndex, CountToDrop);
}

bool UJ1InventoryManager::UseItem(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty())
	{
		return false;
	}

	const UJ1ItemInstance* Item = Slots[SlotIndex].ItemInstance;
	const UJ1ItemTemplate* ItemTemplate = Item->GetItemTemplate();
	if (!ItemTemplate)
	{
		return false;
	}

	// TODO: 실제 사용 효과 발동 (Utility Fragment 등을 찾아서 처리).


	return true;
}

int32 UJ1InventoryManager::GetMaxStackCount(const UJ1ItemInstance* Item)
{
	if (!Item) return 1;

	const UJ1ItemTemplate* ItemTemplate = Item->GetItemTemplate();
	return (ItemTemplate && ItemTemplate->MaxStackCount > 0) ? ItemTemplate->MaxStackCount : 1;
}