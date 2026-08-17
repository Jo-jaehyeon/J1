// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "J1InventorySlotWidget.generated.h"

class UJ1InventoryManager;
class UJ1ItemDragWidget;
class UImage;
class UTextBlock;

// 인벤토리 슬롯 한개를 표현하는 위젯
UCLASS()
class J1_API UJ1InventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitSlot(UJ1InventoryManager* InInventory, int32 InSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefreshVisuals();

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UImage* Img_Icon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* Txt_StackCount;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TWeakObjectPtr<UJ1InventoryManager> Inventory;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 SlotIndex = -1;

	// 드래그 중 마우스를 따라다닐 전용 비주얼 위젯 클래스.
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<class UJ1ItemDragWidget> ItemDragWidgetClass;
};