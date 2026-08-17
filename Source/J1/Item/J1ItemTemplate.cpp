#include "J1ItemTemplate.h"

#include "Types/J1ItemDefine.h"
#include "Item/Fragments/J1ItemFragment.h"
#include "Item/Fragments/J1ItemFragment_Equipable.h"
#include "Item/Fragments/J1ItemFragment_Equipable_Armor.h"
#include "Item/Fragments/J1ItemFragment_Equipable_Utility.h"
#include "Item/Fragments/J1ItemFragment_Equipable_Weapon.h"

UJ1ItemTemplate::UJ1ItemTemplate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR
EDataValidationResult UJ1ItemTemplate::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = UObject::IsDataValid(Context);

	if (SlotCount.X < 1 || SlotCount.Y < 1)
	{
		Context.AddError(FText::FromString(TEXT("SlotCount is less than 1")));
		Result = EDataValidationResult::Invalid;
	}

	if (MaxStackCount < 1)
	{
		Context.AddError(FText::FromString(TEXT("MaxStackCount is less than 1")));
		Result = EDataValidationResult::Invalid;
	}

	const UJ1ItemFragment_Equipable* FoundEquipable = nullptr;
	for (UJ1ItemFragment* Fragment : Fragments)
	{
		if (UJ1ItemFragment_Equipable* CurrentEquipable = Cast<UJ1ItemFragment_Equipable>(Fragment))
		{
			if (FoundEquipable)
			{
				Context.AddError(FText::FromString(TEXT("Duplicated Equipable Fragment")));
				return EDataValidationResult::Invalid;
			}
			FoundEquipable = CurrentEquipable;
		}
	}

	if (FoundEquipable)
	{
		if (FoundEquipable->EquipmentType == EEquipmentType::Count)
		{
			Context.AddError(FText::FromString(TEXT("Equipment Type is Invalid : [EJ1EquipmentType::Count]")));
			return EDataValidationResult::Invalid;
		}

		if (FoundEquipable->EquipmentType == EEquipmentType::Weapon)
		{
			const UJ1ItemFragment_Equipable_Weapon* WeaponFragment = Cast<UJ1ItemFragment_Equipable_Weapon>(FoundEquipable);

			if (WeaponFragment->WeaponType == EWeaponType::Count)
			{
				Context.AddError(FText::FromString(TEXT("Weapon Type is Invalid : [EJ1WeaponType::Count]")));
				Result = EDataValidationResult::Invalid;
			}
		}
		else if (FoundEquipable->EquipmentType == EEquipmentType::Utility)
		{
			const UJ1ItemFragment_Equipable_Utility* UtilityFragment = Cast<UJ1ItemFragment_Equipable_Utility>(FoundEquipable);

			if (UtilityFragment->UtilityType == EUtilityType::Count)
			{
				Context.AddError(FText::FromString(TEXT("Utility Type is Invalid : [EJ1UtilityType::Count]")));
				Result = EDataValidationResult::Invalid;
			}
		}

		if (FoundEquipable->EquipmentType == EEquipmentType::Armor || FoundEquipable->EquipmentType == EEquipmentType::Weapon)
		{
			if (MaxStackCount != 1)
			{
				Context.AddError(FText::FromString(TEXT("Armor or Weapon Type must have MaxStackCount of 1")));
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return Result;
}
#endif // WITH_EDITOR

const UJ1ItemFragment* UJ1ItemTemplate::FindFragmentByClass(TSubclassOf<UJ1ItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		for (UJ1ItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}
	return nullptr;
}