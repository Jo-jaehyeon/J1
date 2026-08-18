#include "Item/J1ItemInstance.h"
#include "Item/J1ItemTemplate.h"
#include "Item/Fragments/J1ItemFragment.h"
#include "Data/J1ItemData.h"

UJ1ItemInstance::UJ1ItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UJ1ItemInstance::Init(int32 InItemTemplateID, EItemRarity InItemRarity)
{
	if (InItemTemplateID <= INDEX_NONE || InItemRarity == EItemRarity::Count)
	{
		return;
	}

	ItemTemplateID = InItemTemplateID;
	ItemRarity = InItemRarity;

	if (const UJ1ItemTemplate* ItemTemplate = GetItemTemplate())
	{
		for (const UJ1ItemFragment* Fragment : ItemTemplate->Fragments)
		{
			if (Fragment)
			{
				Fragment->OnInstanceCreated(this);
			}
		}
	}
}

const UJ1ItemTemplate* UJ1ItemInstance::GetItemTemplate() const
{
	if (ItemTemplateID <= INDEX_NONE)
	{
		return nullptr;
	}

	return &UJ1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
}

void UJ1ItemInstance::AddStat(FName StatKey, int32 StackCount)
{
	StatContainer.AddStack(StatKey, StackCount);
}

void UJ1ItemInstance::RemoveStat(FName StatKey)
{
	StatContainer.RemoveStack(StatKey);
}

EItemRarity UJ1ItemInstance::DetermineItemRarity(const TArray<FJ1ItemRarityProbability>& ItemProbabilities)
{
	float TotalProbability = 0.f;
	for (const FJ1ItemRarityProbability& ItemProbability : ItemProbabilities)
	{
		TotalProbability += ItemProbability.Probability;
	}

	if (TotalProbability > 100.f)
	{
		return EItemRarity::Count;
	}

	float SumProbability = 0.f;
	const float RandomValue = FMath::RandRange(0.f, 100.f);

	for (const FJ1ItemRarityProbability& ItemProbability : ItemProbabilities)
	{
		SumProbability += ItemProbability.Probability;
		if (RandomValue < SumProbability)
		{
			return ItemProbability.Rarity;
		}
	}

	return EItemRarity::Count;
}

bool UJ1ItemInstance::HasStat(FName StatKey) const
{
	return StatContainer.ContainsKey(StatKey);
}

int32 UJ1ItemInstance::GetStackCountByKey(FName StatKey) const
{
	return StatContainer.GetStackCount(StatKey);
}

bool UJ1ItemInstance::CanStackWith(const UJ1ItemInstance* Other) const
{
	if (!Other) return false;
	if (ItemTemplateID <= INDEX_NONE || ItemTemplateID != Other->GetItemTemplateID()) return false;
	if (ItemRarity != Other->GetItemRarity()) return false;
	return true;
}

const UJ1ItemFragment* UJ1ItemInstance::FindFragmentByClass(TSubclassOf<UJ1ItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		if (const UJ1ItemTemplate* ItemTemplate = GetItemTemplate())
		{
			return ItemTemplate->FindFragmentByClass(FragmentClass);
		}
	}
	return nullptr;
}