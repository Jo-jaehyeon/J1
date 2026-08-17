// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Types/J1ItemDefine.h"
#include "J1ItemFragment_Equipable_Attachment.h"
#include "J1ItemFragment_Equipable_Weapon.generated.h"

UCLASS()
class UJ1ItemFragment_Equipable_Weapon : public UJ1ItemFragment_Equipable_Attachment
{
	GENERATED_BODY()

public:
	UJ1ItemFragment_Equipable_Weapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA

public:
	virtual void OnInstanceCreated(UJ1ItemInstance* ItemInstance) const override;

public:
	UPROPERTY(EditDefaultsOnly)
	EWeaponType WeaponType = EWeaponType::Count;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> AttackSwingSound;

	// 원본의 SkillAbilitySets(GAS) 제거. 무기 고유 스킬이 필요하면
	// 프로젝트 자체 스킬 시스템에 맞는 타입으로 다시 추가하세요.

	UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow))
	TArray<FJ1RarityStatRangeSet> RarityStatRangeSets;
};