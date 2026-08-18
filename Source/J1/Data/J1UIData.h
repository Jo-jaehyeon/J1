// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Types/J1EnumTypes.h"
#include "Types/J1ItemDefine.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "J1UIData.generated.h"

class UImage;
//class UJ1ItemDragWidget;
//class UJ1ItemHoverWidget;
//class UJ1InventorySlotWidget;
//class UJ1InventoryEntryWidget;

USTRUCT(BlueprintType)
struct FJ1ItemRarityInfoEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (HideAlphaChannel))
	FColor Color = FColor::White;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> EntryTexture;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> HoverTexture;
};


USTRUCT(BlueprintType)
struct FJ1UIInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly)
	FText Title;

	UPROPERTY(EditDefaultsOnly)
	FText Content;
};


UCLASS()
class J1_API UJ1UIData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UJ1UIData& Get();

public:
	UTexture2D* GetEntryRarityTexture(EItemRarity ItemRarity) const;
	UTexture2D* GetHoverRarityTexture(EItemRarity ItemRarity) const;
	FColor GetRarityColor(EItemRarity ItemRarity) const;

	const FJ1UIInfo& GetTagUIInfo(FGameplayTag Tag) const;

public:
	UPROPERTY(EditDefaultsOnly)
	FIntPoint UnitInventorySlotSize = FIntPoint::ZeroValue;

public:
	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<UJ1ItemDragWidget> DragWidgetClass;

	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<UJ1ItemHoverWidget> ItemHoverWidgetClass;

	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<UJ1InventorySlotWidget> InventorySlotWidgetClass;
	//
	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<UJ1InventoryEntryWidget> InventoryEntryWidgetClass;

private:
	UPROPERTY(EditDefaultsOnly, meta = (ArraySizeEnum = "EItemRarity"))
	FJ1ItemRarityInfoEntry RarityInfoEntries[(int32)EItemRarity::Count];

	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Tag UI Infos"))
	TMap<FGameplayTag, FJ1UIInfo> TagUIInfos;
	
};
