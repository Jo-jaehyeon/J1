// Copyright © 2026 Jerry. All rights reserved.

#include "J1ItemFragment_Equipable_Utility.h"
#include "UObject/ObjectSaveContext.h"

UJ1ItemFragment_Equipable_Utility::UJ1ItemFragment_Equipable_Utility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EquipmentType = EEquipmentType::Utility;
}

#if WITH_EDITORONLY_DATA
void UJ1ItemFragment_Equipable_Utility::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	for (int32 i = 0; i < RarityStatSets.Num(); i++)
	{
		TArray<FJ1RarityStat>& RarityStats = RarityStatSets[i].RarityStats;
		RarityStats.SetNum((int32)EItemRarity::Count);
		for (int32 j = 0; j < RarityStats.Num(); j++)
		{
			RarityStats[j].Rarity = (EItemRarity)j;
		}
	}
}
#endif // WITH_EDITORONLY_DATA

void UJ1ItemFragment_Equipable_Utility::OnInstanceCreated(UJ1ItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);

	AddStatTagStack(ItemInstance, RarityStatSets);
}