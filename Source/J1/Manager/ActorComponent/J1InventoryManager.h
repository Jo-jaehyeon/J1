// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Item/J1ItemInstance.h"
#include "J1InventoryManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotChanged, int32, SlotIndex);

/**
 * 인벤토리 슬롯 하나. 
 * 스택 개수는 인스턴스가 아니라 슬롯
 */
USTRUCT(BlueprintType)
struct FJ1InventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UJ1ItemInstance> ItemInstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 StackCount = 0;

	bool IsEmpty() const { return ItemInstance == nullptr || StackCount <= 0; }

	void Reset()
	{
		ItemInstance = nullptr;
		StackCount = 0;
	}
};

UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class J1_API UJ1InventoryManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UJ1InventoryManager();

protected:
	virtual void BeginPlay() override;

public:
	// 개발 중 크기 조절 / 런타임 가방 업그레이드 둘 다에 사용
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ResizeInventory(int32 NewColumns, int32 NewRows);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetSlotCount() const { return Slots.Num(); }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FJ1InventorySlot GetSlot(int32 Index) const;

	// 기존 스택에 먼저 병합을 시도하고, 남으면 빈 슬롯에 채움. 리턴값 = 못 넣고 남은 개수(0=전량 성공)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItem(UJ1ItemInstance* NewItem, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemAt(int32 SlotIndex, int32 Count = 1);

	// 드래그로 슬롯 이동. 대상이 비어있으면 이동, 같은 아이템이면 병합, 다르면 스왑
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool MoveItem(int32 FromIndex, int32 ToIndex);

	// 인벤토리 바깥으로 드래그했을 때 호출 (버리기)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool DropItem(int32 SlotIndex, int32 DropCount = -1);

	// 우클릭 사용
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItem(int32 SlotIndex);

private:
	// MaxStackCount는 아이템 인스턴스가 아니라 템플릿에 있는 값이라 매번 조회해야 한다.
	static int32 GetMaxStackCount(const UJ1ItemInstance* Item);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "1"))
	int32 Columns = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "1"))
	int32 Rows = 5;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSlotChanged OnSlotChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FJ1InventorySlot> Slots;
};