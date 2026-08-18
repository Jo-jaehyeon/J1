// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "J1InventoryWidget.generated.h"

class UJ1InventoryManager;
class UJ1InventorySlotWidget;
class UUniformGridPanel;

//  인벤토리 전체를 보여주는 위젯.
UCLASS()
class J1_API UJ1InventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 인벤토리를 열 때 호출: HUD/플레이어 컨트롤러 등에서
	// InventoryWidget->InitInventory(PlayerInventoryComponent);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitInventory(UJ1InventoryManager* InInventory);

protected:
	// Columns/Rows에 맞춰 슬롯 위젯을 새로 만들고 그리드에 배치
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RebuildGrid();

	UFUNCTION()	void HandleInventoryChanged();
	UFUNCTION()	void HandleSlotChanged(int32 SlotIndex);


public:
	// 슬롯 위젯 클래스는 보통 BP_InventorySlotWidget(UInventorySlotWidget 상속) 지정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<UJ1InventorySlotWidget> SlotWidgetClass;

protected:
	// 위젯 블루프린트에서 이름을 "GridPanel"로 맞춰서 UniformGridPanel 배치
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UUniformGridPanel* UGP_ItemSlots;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TWeakObjectPtr<UJ1InventoryManager> Inventory;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<UJ1InventorySlotWidget*> SlotWidgets;
};