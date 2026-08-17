// Copyright © 2026 Jerry. All rights reserved.

#include "Data/J1ItemData.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // WITH_EDITOR

#include "Item/J1ItemTemplate.h"
#include "Item/Fragments/J1ItemFragment_Equipable_Armor.h"
#include "Item/Fragments/J1ItemFragment_Equipable_Weapon.h"
#include "System/J1AssetManager.h"
#include "UObject/ObjectSaveContext.h"

const UJ1ItemData& UJ1ItemData::Get()
{
	return UJ1AssetManager::Get().GetItemData();
}

#if WITH_EDITORONLY_DATA
void UJ1ItemData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	ItemTemplateIDToClass.KeySort([](const int32 A, const int32 B)
		{
			return A < B;
		});
	
	ItemTemplateClassToID.Empty();
	WeaponItemTemplateClasses.Empty();
	ArmorItemTemplateClasses.Empty();

	for (const auto& Pair : ItemTemplateIDToClass)
	{
		ItemTemplateClassToID.Emplace(Pair.Value, Pair.Key);
	
		const UJ1ItemTemplate* ItemTemplate = Pair.Value.GetDefaultObject();
		//if (const UJ1ItemFragment_Equipable_Weapon* WeaponFragment = ItemTemplate->FindFragmentByClass<UJ1ItemFragment_Equipable_Weapon>())
		//{
		//	if (WeaponFragment->WeaponType != EWeaponType::Unarmed)
		//	{
		//		WeaponItemTemplateClasses.Add(Pair.Value);
		//	}
		//}
		//else if (ItemTemplate->FindFragmentByClass<UJ1ItemFragment_Equipable_Armor>())
		//{
		//	ArmorItemTemplateClasses.Add(Pair.Value);
		//}
	}	
}
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITOR
EDataValidationResult UJ1ItemData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	TSet<int32> ItemTemplateIDSet;
	TSet<TSubclassOf<UJ1ItemTemplate>> ItemTemplateClassSet;

	for (const auto& Pair : ItemTemplateIDToClass)
	{
		// ID Check
		const int32 ItemTemplateID = Pair.Key;
	
		if (ItemTemplateID <= 0)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid ID : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
	
		if (ItemTemplateIDSet.Contains(ItemTemplateID))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated ID : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
	
		ItemTemplateIDSet.Add(ItemTemplateID);
	
		// Class Check
		const TSubclassOf<UJ1ItemTemplate> ItemTemplateClass = Pair.Value;
	
		if (ItemTemplateClass == nullptr)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid Class : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
	
		if (ItemTemplateClassSet.Contains(ItemTemplateClass))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated Class : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
	
		ItemTemplateClassSet.Add(ItemTemplateClass);
	}
	return Result;
}
#endif // WITH_EDITOR

const UJ1ItemTemplate& UJ1ItemData::FindItemTemplateByID(int32 ItemTemplateID) const
{
	const TSubclassOf<UJ1ItemTemplate>* ItemDefinitionClass = ItemTemplateIDToClass.Find(ItemTemplateID);
	ensureAlwaysMsgf(ItemDefinitionClass, TEXT("Can't find ItemTemplateClass from ID [%d]"), ItemTemplateID);
	return *(ItemDefinitionClass->GetDefaultObject());
}

int32 UJ1ItemData::FindItemTemplateIDByClass(TSubclassOf<UJ1ItemTemplate> ItemTemplateClass) const
{
	const int32* ItemTemplateID = ItemTemplateClassToID.Find(ItemTemplateClass);
	ensureAlwaysMsgf(ItemTemplateID, TEXT("Can't find ItemTemplateID from Class"));
	return *ItemTemplateID;
}

void UJ1ItemData::GetAllItemTemplateClasses(TArray<TSubclassOf<UJ1ItemTemplate>>& OutItemTemplateClasses) const
{
	OutItemTemplateClasses.Reset();
	OutItemTemplateClasses.Reserve(ItemTemplateIDToClass.Num());

	for (auto& Pair : ItemTemplateIDToClass)
	{
		OutItemTemplateClasses.Add(Pair.Value);
	}
}