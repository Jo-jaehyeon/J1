// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "J1ItemFragment_Equipable.h"
#include "J1ItemFragment_Equipable_Attachment.generated.h"

class AJ1ItemBase;

USTRUCT(BlueprintType)
struct FJ1ItemAttachInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AJ1ItemBase> SpawnItemClass;

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocket;

	UPROPERTY(EditDefaultsOnly)
	FTransform AttachTransform;
};

/**
 * 캐릭터 몸에 실제로 부착되는(무기, 유틸리티 등) 장비 공통 데이터.
 * 방어구(Armor)는 메쉬만 바꿔 끼우는 개념이라 이쪽을 상속하지 않고
 * UJ1ItemFragment_Equipable을 바로 상속한다.
 */
UCLASS(Abstract, Const)
class UJ1ItemFragment_Equipable_Attachment : public UJ1ItemFragment_Equipable
{
	GENERATED_BODY()

public:
	UJ1ItemFragment_Equipable_Attachment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly)
	FJ1ItemAttachInfo ItemAttachInfo;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> EquipMontage;
};