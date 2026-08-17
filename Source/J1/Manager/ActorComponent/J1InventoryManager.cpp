// Copyright © 2026 Jerry. All rights reserved.

#include "J1InventoryManager.h"
#include "Item/J1ItemDefinition.h"

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

FInventorySlot UJ1InventoryManager::GetSlot(int32 Index) const
{
	return Slots.IsValidIndex(Index) ? Slots[Index] : FInventorySlot();
}

int32 UJ1InventoryManager::AddItem(const FJ1ItemInstance& NewItem)
{
	if (!NewItem.IsValid())
	{
		return 0;
	}

	int32 Remaining = NewItem.StackCount;
	const UJ1ItemDefinition* Def = NewItem.ItemDefinition.LoadSynchronous();
	const int32 MaxStack = (Def && Def->bStackable) ? Def->MaxStackSize : 1;

	// 1) 기존 스택에 우선 병합
	if (Def && Def->bStackable)
	{
		for (FInventorySlot& Slot : Slots)
		{
			if (Remaining <= 0) break;
			if (Slot.IsEmpty()) continue;
			if (!Slot.Item.CanStackWith(NewItem)) continue;

			const int32 Space = MaxStack - Slot.Item.StackCount;
			if (Space <= 0) continue;

			const int32 ToMove = FMath::Min(Space, Remaining);
			Slot.Item.StackCount += ToMove;
			Remaining -= ToMove;
		}
	}

	// 2) 남은 수량은 빈 슬롯에 새로 배치 (여러 슬롯에 나눠 담김)
	for (int32 i = 0; i < Slots.Num() && Remaining > 0; ++i)
	{
		if (!Slots[i].IsEmpty()) continue;

		FJ1ItemInstance Split = NewItem;
		Split.InstanceGuid = FGuid::NewGuid();
		Split.StackCount = FMath::Min(MaxStack, Remaining);
		Slots[i].Item = Split;
		Remaining -= Split.StackCount;
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

	FInventorySlot& Slot = Slots[SlotIndex];
	Slot.Item.StackCount -= Count;
	if (Slot.Item.StackCount <= 0)
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

	FInventorySlot& From = Slots[FromIndex];
	FInventorySlot& To = Slots[ToIndex];

	if (To.IsEmpty())
	{
		To.Item = From.Item;
		From.Reset();
	}
	else if (To.Item.CanStackWith(From.Item))
	{
		const UJ1ItemDefinition* Def = To.Item.ItemDefinition.LoadSynchronous();
		const int32 MaxStack = Def ? Def->MaxStackSize : 1;
		const int32 Space = MaxStack - To.Item.StackCount;
		const int32 ToMove = FMath::Min(Space, From.Item.StackCount);

		To.Item.StackCount += ToMove;
		From.Item.StackCount -= ToMove;
		if (From.Item.StackCount <= 0)
		{
			From.Reset();
		}
	}
	else
	{
		// 다른 아이템, 혹은 스택 불가(장비) -> 위치 스왑
		Swap(From.Item, To.Item);
	}

	OnSlotChanged.Broadcast(FromIndex);
	OnSlotChanged.Broadcast(ToIndex);
	OnInventoryChanged.Broadcast();
	return true;
}

bool UJ1InventoryManager::DropItem(int32 SlotIndex, int32 DropCount)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty())
	{
		return false;
	}

	const int32 CountToDrop = (DropCount < 0)
		? Slots[SlotIndex].Item.StackCount
		: FMath::Min(DropCount, Slots[SlotIndex].Item.StackCount);

	// TODO: 여기서 실제 월드 픽업 액터를 스폰.
	// 예) GetWorld()->SpawnActor<AItemPickup>(...)->InitFromItemInstance(Slots[SlotIndex].Item, CountToDrop);

	return RemoveItemAt(SlotIndex, CountToDrop);
}

bool UJ1InventoryManager::UseItem(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty())
	{
		return false;
	}

	const UJ1ItemDefinition* Def = Slots[SlotIndex].Item.ItemDefinition.LoadSynchronous();
	if (!Def)
	{
		return false;
	}

	// TODO: 실제 사용 효과 발동.
	// Def에 UItemUseEffect 같은 자산을 참조시키거나, BlueprintImplementableEvent로
	// 디자이너가 C++ 안 건드리고 효과를 만들 수 있게 하는 걸 추천.

	if (Def->ItemType == EItemType::Consumable)
	{
		RemoveItemAt(SlotIndex, 1);
	}
	return true;
}