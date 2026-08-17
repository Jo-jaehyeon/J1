// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/DragDropOperation.h"
#include "J1DragDrop.generated.h"

class UJ1InventoryManager;

UCLASS()
class J1_API UJ1DragDrop : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	int32 SourceSlotIndex = -1;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	TWeakObjectPtr<UJ1InventoryManager> SourceInventory;
};