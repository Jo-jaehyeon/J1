// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "J1InventorySlotWidget.generated.h"

class UJ1InventoryManager;
class UJ1ItemDragWidget;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotClicked, UJ1InventoryManager*, Inventory, int32, SlotIndex);

// 인벤토리 슬롯 한개를 표현하는 위젯
UCLASS()
class J1_API UJ1InventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void   NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool   NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void   NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitSlot(UJ1InventoryManager* InInventory, int32 InSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefreshVisuals();

public:
	// 아이템 등록 UI 등 외부에서 "이 슬롯이 클릭됐다"는 걸 알아야 할 때 구독.
	// 드래그 여부와 무관하게 좌클릭 시점에 항상 브로드캐스트된다.
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSlotClicked OnSlotClicked;
protected:
	// ════════════════════════════════════
	//              UMG 바인딩
	// ════════════════════════════════════
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UImage*		Img_Icon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock* Txt_StackCount;


	// ════════════════════════════════════
	//           Member Variable
	// ════════════════════════════════════
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TWeakObjectPtr<UJ1InventoryManager> Inventory;

	// 드래그 중 마우스를 따라다닐 전용 비주얼 위젯 클래스.
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<class UJ1ItemDragWidget> ItemDragWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 SlotIndex = -1;
};