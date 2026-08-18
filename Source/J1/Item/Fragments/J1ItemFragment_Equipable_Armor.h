// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "J1ItemFragment_Equipable.h"
#include "J1ItemFragment_Equipable_Armor.generated.h"

UCLASS()
class UJ1ItemFragment_Equipable_Armor : public UJ1ItemFragment_Equipable
{
	GENERATED_BODY()

public:
	UJ1ItemFragment_Equipable_Armor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA

public:
	virtual void OnInstanceCreated(UJ1ItemInstance* ItemInstance) const override;

public:
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> ArmorMesh;

	UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow))
	TArray<FJ1RarityStatRangeSet> RarityStatRangeSets;
};