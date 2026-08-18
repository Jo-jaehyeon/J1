// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Types/J1ItemDefine.h"
#include "J1ItemInstance.generated.h"

class UJ1ItemFragment;
class UJ1ItemTemplate;



USTRUCT(BlueprintType)
struct FJ1ItemRarityProbability
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	EItemRarity Rarity = EItemRarity::Poor;

	UPROPERTY(EditAnywhere)
	float Probability = 0;
};

/**
 * 아이템 "개체" 데이터. 어떤 템플릿(ItemTemplateID)에서 나왔는지 + 희귀도 
 */
UCLASS(BlueprintType)
class UJ1ItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UJ1ItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(int32 InItemTemplateID, EItemRarity InItemRarity);

public:
	UFUNCTION(BlueprintCallable)
	void AddStat(FName StatKey, int32 StackCount);

	UFUNCTION(BlueprintCallable)
	void RemoveStat(FName StatKey);

public:
	static EItemRarity DetermineItemRarity(const TArray<FJ1ItemRarityProbability>& ItemProbabilities);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetItemTemplateID() const { return ItemTemplateID; }

	// UJ1ItemData 레지스트리를 통해 실제 UJ1ItemTemplate(블루프린트 클래스 CDO)을 찾아온다.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const UJ1ItemTemplate* GetItemTemplate() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EItemRarity GetItemRarity() const { return ItemRarity; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasStat(FName StatKey) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetStackCountByKey(FName StatKey) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FJ1ItemStatContainer& GetStatContainer() const { return StatContainer; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanStackWith(const UJ1ItemInstance* Other) const;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = "false", meta = (DeterminesOutputType = "FragmentClass"))
	const UJ1ItemFragment* FindFragmentByClass(TSubclassOf<UJ1ItemFragment> FragmentClass) const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const
	{
		return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass());
	}

private:
	UPROPERTY() int32 ItemTemplateID = INDEX_NONE;
	UPROPERTY() EItemRarity ItemRarity = EItemRarity::Poor;
	UPROPERTY() FJ1ItemStatContainer StatContainer;
};