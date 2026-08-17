// Copyright © 2026 Jerry. All rights reserved.

#include "J1ItemBase.h"

#include "Data/J1ItemData.h"
#include "Item/J1ItemTemplate.h"
#include "Item/Fragments/J1ItemFragment_Equipable_Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Types/J1LogChannels.h"


AJ1ItemBase::AJ1ItemBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("ItemMesh");
	MeshComponent->SetupAttachment(GetRootComponent());
}

void AJ1ItemBase::BeginPlay()
{
	Super::BeginPlay();
}

void AJ1ItemBase::Init(int32 InTemplateID, EItemRarity InItemRarity)
{
	TemplateID = InTemplateID;
	ItemRarity = InItemRarity;
}

UAnimMontage* AJ1ItemBase::GetEquipMontage()
{
	UAnimMontage* EquipMontage = nullptr;

	if (TemplateID > 0)
	{
		const UJ1ItemTemplate& ItemTemplate = UJ1ItemData::Get().FindItemTemplateByID(TemplateID);
		if (const UJ1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UJ1ItemFragment_Equipable_Attachment>())
		{
			//EquipMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->EquipMontage);
		}
	}

	return EquipMontage;
}