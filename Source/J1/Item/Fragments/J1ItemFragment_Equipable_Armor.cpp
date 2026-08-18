// Copyright © 2026 Jerry. All rights reserved.

#include "J1ItemFragment_Equipable_Armor.h"
#include "Item/J1ItemInstance.h"
#include "UObject/ObjectSaveContext.h"

UJ1ItemFragment_Equipable_Armor::UJ1ItemFragment_Equipable_Armor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EquipmentType = EEquipmentType::Armor;
}

#if WITH_EDITORONLY_DATA
void UJ1ItemFragment_Equipable_Armor::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	for (int32 i = 0; i < RarityStatRangeSets.Num(); i++)
	{
		TArray<FJ1RarityStatRange>& RarityStatRanges = RarityStatRangeSets[i].RarityStatRanges;
		RarityStatRanges.SetNum((int32)EItemRarity::Count);
		for (int32 j = 0; j < RarityStatRanges.Num(); j++)
		{
			RarityStatRanges[j].Rarity = (EItemRarity)j;
		}
	}
}
#endif // WITH_EDITORONLY_DATA

void UJ1ItemFragment_Equipable_Armor::OnInstanceCreated(UJ1ItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);

	AddStatTagStack(ItemInstance, RarityStatRangeSets);
}