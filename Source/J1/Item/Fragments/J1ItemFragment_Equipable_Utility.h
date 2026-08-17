// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Types/J1ItemDefine.h"
#include "J1ItemFragment_Equipable_Attachment.h"
#include "J1ItemFragment_Equipable_Utility.generated.h"

UCLASS()
class UJ1ItemFragment_Equipable_Utility : public UJ1ItemFragment_Equipable_Attachment
{
	GENERATED_BODY()

public:
	UJ1ItemFragment_Equipable_Utility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA

public:
	virtual void OnInstanceCreated(UJ1ItemInstance* ItemInstance) const override;

public:
	UPROPERTY(EditDefaultsOnly)
	EUtilityType UtilityType = EUtilityType::Count;

	// 원본의 UtilityEffectClasses(UGameplayEffect, GAS) 제거.
	// 사용 효과가 필요하면 프로젝트 자체 시스템에 맞는 타입으로 다시 추가하세요.

	UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow))
	TArray<FJ1RarityStatSet> RarityStatSets;
};