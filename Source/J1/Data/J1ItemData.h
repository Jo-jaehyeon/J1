// Copyright © 2026 Jerry. All rights reserved.

#pragma once
#include "Engine/DataAsset.h"
#include "J1ItemData.generated.h"

class UJ1ItemTemplate;

UCLASS(BlueprintType, Const, meta = (DisplayName = "J1 Item Data"))
class J1_API UJ1ItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UJ1ItemData& Get();

public:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

public:
	const UJ1ItemTemplate& FindItemTemplateByID(int32 ItemTemplateID) const;
	int32 FindItemTemplateIDByClass(TSubclassOf<UJ1ItemTemplate> ItemTemplateClass) const;
	
	void GetAllItemTemplateClasses(TArray<TSubclassOf<UJ1ItemTemplate>>& OutItemTemplateClasses) const;
	const TArray<TSubclassOf<UJ1ItemTemplate>>& GetWeaponItemTemplateClasses() const { return WeaponItemTemplateClasses; }
	const TArray<TSubclassOf<UJ1ItemTemplate>>& GetArmorItemTemplateClasses() const { return ArmorItemTemplateClasses; }

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<int32, TSubclassOf<UJ1ItemTemplate>> ItemTemplateIDToClass;
	
	UPROPERTY()
	TMap<TSubclassOf<UJ1ItemTemplate>, int32> ItemTemplateClassToID;

private:
	UPROPERTY()
	TArray<TSubclassOf<UJ1ItemTemplate>> WeaponItemTemplateClasses;
	
	UPROPERTY()
	TArray<TSubclassOf<UJ1ItemTemplate>> ArmorItemTemplateClasses;
};
