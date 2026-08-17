// Copyright © 2026 Jerry. All rights reserved.


#include "Data/J1UIData.h"
#include "Engine/AssetManager.h"
#include "System/J1AssetManager.h"

const UJ1UIData& UJ1UIData::Get()
{
	return UJ1AssetManager::Get().GetUIData();
}

UTexture2D* UJ1UIData::GetEntryRarityTexture(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
		return nullptr;

	return RarityInfoEntries[ItemRarityIndex].EntryTexture;
}

UTexture2D* UJ1UIData::GetHoverRarityTexture(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
		return nullptr;

	return RarityInfoEntries[ItemRarityIndex].HoverTexture;
}

FColor UJ1UIData::GetRarityColor(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
		return FColor::White;

	return RarityInfoEntries[ItemRarityIndex].Color;
}

const FJ1UIInfo& UJ1UIData::GetTagUIInfo(FGameplayTag Tag) const
{
	const FJ1UIInfo* UIInfo = TagUIInfos.Find(Tag);
	if (UIInfo == nullptr)
	{
		static FJ1UIInfo EmptyInfo;
		return EmptyInfo;
	}

	return *UIInfo;
}
