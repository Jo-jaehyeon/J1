// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Item/Fragments/J1ItemFragment.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // WITH_EDITOR
#include "J1ItemTemplate.generated.h"

class UJ1ItemInstance;
class UJ1ItemFragment;

/**
 * 아이템 "원본" 데이터. DataAsset이 아니라 Blueprintable UObject로 두고,
 * 아이템 종류마다 이 클래스를 상속하는 블루프린트(BP_Item_LongSword 등)를 만들어 사용한다.
 * ID <-> 클래스 매핑은 이 클래스가 직접 하지 않고 UJ1ItemData가 전담한다.
 * Fragments에 필요한 기능 조각(장비/무기/방어구 등)을 EditInlineNew로 꽂아넣는 구조.
 */
UCLASS(Blueprintable, Const, Abstract)
class UJ1ItemTemplate : public UObject
{
	GENERATED_BODY()

public:
	UJ1ItemTemplate(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = "false", meta = (DeterminesOutputType = "FragmentClass"))
	const UJ1ItemFragment* FindFragmentByClass(TSubclassOf<UJ1ItemFragment> FragmentClass) const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const
	{
		return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass());
	}

public:
	// 경매장/인벤토리 그리드에서 차지하는 칸 크기
	UPROPERTY(EditDefaultsOnly)
	FIntPoint SlotCount = FIntPoint(1, 1);

	UPROPERTY(EditDefaultsOnly)
	int32 MaxStackCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> PickupableMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<UJ1ItemFragment>> Fragments;
};