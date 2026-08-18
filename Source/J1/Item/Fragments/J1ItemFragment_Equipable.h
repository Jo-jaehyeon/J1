// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Types/J1ItemDefine.h"
#include "J1ItemFragment.h"
#include "J1ItemFragment_Equipable.generated.h"

USTRUCT(BlueprintType)
struct FJ1RarityStat
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EItemRarity Rarity = EItemRarity::Poor;

	UPROPERTY(EditDefaultsOnly)
	int32 Value = 0;
};

USTRUCT(BlueprintType)
struct FJ1RarityStatSet
{
	GENERATED_BODY()

public:
	FJ1RarityStatSet();

public:
	UPROPERTY(EditDefaultsOnly)
	FName StatKey;

	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FJ1RarityStat> RarityStats;
};

USTRUCT(BlueprintType)
struct FJ1RarityStatRange
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EItemRarity Rarity = EItemRarity::Poor;

	UPROPERTY(EditDefaultsOnly)
	int32 MinValue = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxValue = 0;
};

USTRUCT(BlueprintType)
struct FJ1RarityStatRangeSet
{
	GENERATED_BODY()

public:
	FJ1RarityStatRangeSet();

public:
	UPROPERTY(EditDefaultsOnly)
	FName StatKey;

	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FJ1RarityStatRange> RarityStatRanges;
};

/**
 * 장비류(무기/방어구/유틸리티) 공통 베이스.
 * 원본의 BaseAbilitySet(ULyraAbilitySet)은 GAS 종속이라 제거.
 * 장비 장착 시 실제 효과를 적용하려면 프로젝트 자체 시스템으로 별도 구현 필요.
 */
UCLASS(Abstract, Const)
class UJ1ItemFragment_Equipable : public UJ1ItemFragment
{
	GENERATED_BODY()

public:
	UJ1ItemFragment_Equipable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	void AddStatTagStack(UJ1ItemInstance* ItemInstance, const TArray<FJ1RarityStatSet>& RarityStatSets) const;
	void AddStatTagStack(UJ1ItemInstance* ItemInstance, const TArray<FJ1RarityStatRangeSet>& RarityStatRangeSets) const;

public:
	EEquipmentType EquipmentType = EEquipmentType::Count;
};