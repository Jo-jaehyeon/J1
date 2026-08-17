#pragma once

#include "CoreMinimal.h"
#include "J1ItemDefine.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Poor,
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary,
	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Consumable	UMETA(DisplayName = "Consumable"),
	Equipment	UMETA(DisplayName = "Equipment"),
	Material	UMETA(DisplayName = "Material"),
	Quest		UMETA(DisplayName = "Quest"),
	Misc		UMETA(DisplayName = "Misc")
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Weapon,
	Armor,
	Utility,
	Count UMETA(Hidden)
};

// 무기 세부 타입 - 프로젝트에 맞게 항목 추가/수정
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Sword,
	Bow,
	Staff,
	Count UMETA(Hidden)
};

// 유틸리티 아이템 세부 타입 - 프로젝트에 맞게 항목 추가/수정
UENUM(BlueprintType)
enum class EUtilityType : uint8
{
	Consumable,
	Throwable,
	Count UMETA(Hidden)
};

/**
 * 원본 프로젝트의 FA1GameplayTagStackContainer를 대체.
 * FFastArraySerializer 기반 리플리케이션 로직을 걷어내고,
 * GameplayTags 플러그인 의존성도 없앤 순수 FName-int32 맵으로 단순화했다.
 * 계층형 태그 매칭(부모 태그로 자식들 한번에 필터링 등)이 필요해지면
 * 그때 FGameplayTag로 되돌리면 된다.
 */
USTRUCT(BlueprintType)
struct FJ1ItemStatContainer
{
	GENERATED_BODY()

public:
	void AddStack(FName StatKey, int32 StackCount);
	void RemoveStack(FName StatKey);

	int32 GetStackCount(FName StatKey) const;
	bool ContainsKey(FName StatKey) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TMap<FName, int32> StatMap;
};