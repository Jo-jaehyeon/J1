// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Types/J1ItemDefine.h"
#include "J1ItemBase.generated.h"

class USkeletalMeshComponent;

UCLASS(BlueprintType, Abstract)
class AJ1ItemBase : public AActor
{
	GENERATED_BODY()

public:
	AJ1ItemBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:
	void Init(int32 InTemplateID, EItemRarity InItemRarity);

public:
	int32 GetTemplateID() const { return TemplateID; }
	EItemRarity GetItemRarity() const { return ItemRarity; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAnimMontage* GetEquipMontage();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

protected:
	UPROPERTY(EditAnyWhere)
	int32 TemplateID;

	UPROPERTY(EditAnyWhere)
	EItemRarity ItemRarity = EItemRarity::Count;
};
