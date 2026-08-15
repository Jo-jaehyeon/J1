// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/DragDropOperation.h"
#include "J1ItemDragDrop.generated.h"

/**
 * 
 */
UCLASS()
class J1_API UJ1ItemDragDrop : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UJ1ItemDragDrop(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
