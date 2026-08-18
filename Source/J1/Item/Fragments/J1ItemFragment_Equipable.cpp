// Copyright © 2026 Jerry. All rights reserved.

#include "J1ItemFragment_Equipable.h"
#include "Item/J1ItemInstance.h"

FJ1RarityStatSet::FJ1RarityStatSet()
{
	RarityStats.SetNum((int32)EItemRarity::Count);
	for (int32 i = 0; i < RarityStats.Num(); i++)
	{
		RarityStats[i].Rarity = (EItemRarity)i;
	}
}

FJ1RarityStatRangeSet::FJ1RarityStatRangeSet()
{
	RarityStatRanges.SetNum((int32)EItemRarity::Count);
	for (int32 i = 0; i < RarityStatRanges.Num(); i++)
	{
		RarityStatRanges[i].Rarity = (EItemRarity)i;
	}
}

UJ1ItemFragment_Equipable::UJ1ItemFragment_Equipable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UJ1ItemFragment_Equipable::AddStatTagStack(UJ1ItemInstance* ItemInstance, const TArray<FJ1RarityStatSet>& RarityStatSets) const
{
	if (ItemInstance == nullptr) return;

	for (const FJ1RarityStatSet& RarityStatSet : RarityStatSets)
	{
		const FName& StatKey = RarityStatSet.StatKey;
		const FJ1RarityStat& Stat = RarityStatSet.RarityStats[(int32)ItemInstance->GetItemRarity()];
		ItemInstance->AddStat(StatKey, Stat.Value);
	}
}

void UJ1ItemFragment_Equipable::AddStatTagStack(UJ1ItemInstance* ItemInstance, const TArray<FJ1RarityStatRangeSet>& RarityStatRangeSets) const
{
	if (ItemInstance == nullptr) return;

	for (const FJ1RarityStatRangeSet& RarityStatRangeSet : RarityStatRangeSets)
	{
		const FName& StatKey = RarityStatRangeSet.StatKey;
		const FJ1RarityStatRange& StatRange = RarityStatRangeSet.RarityStatRanges[(int32)ItemInstance->GetItemRarity()];
		const int32 StatValue = FMath::RandRange(StatRange.MinValue, StatRange.MaxValue);
		ItemInstance->AddStat(StatKey, StatValue);
	}
}