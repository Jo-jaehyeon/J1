// Copyright © 2026 Jerry. All rights reserved.

#include "Character/PC/J1CustomizePreviewActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

AJ1CustomizePreviewActor::AJ1CustomizePreviewActor()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	//CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//FollowCamera->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void AJ1CustomizePreviewActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AJ1CustomizePreviewActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AJ1CustomizePreviewActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AJ1CustomizePreviewActor::ApplyClass(const FCharacterSkinBaseData& ClassData)
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	TArray<FSoftObjectPath> AssetsToLoad;
	AssetsToLoad.Add(ClassData.SkeletalMesh.ToSoftObjectPath());
	AssetsToLoad.Add(ClassData.AnimBlueprint.ToSoftObjectPath());

	Streamable.RequestAsyncLoad(
		AssetsToLoad,
		FStreamableDelegate::CreateUObject(
			this,
			&AJ1CustomizePreviewActor::OnSkeletalMeshLoaded,
			ClassData.SkeletalMesh,
			ClassData.AnimBlueprint
		)
	);
}

void AJ1CustomizePreviewActor::ApplySkin(ESkinSlot SkinSlot, const FSkinEntry& SkinData)
{
	const int32 SlotIndex = GetMaterialIndexForSlot(SkinSlot);

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		SkinData.Material.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(
			this,
			&AJ1CustomizePreviewActor::OnMaterialLoaded,
			SkinData.Material,
			SlotIndex
		)
	);
}

/** 슬롯 → 머터리얼 인덱스 매핑 */
int32 AJ1CustomizePreviewActor::GetMaterialIndexForSlot(ESkinSlot Slot)
{

	//Warrior
	// Upper - 0,1, lower - 2, weapon-3

	switch (Slot)
	{
	case ESkinSlot::Upper:  return 0;
	case ESkinSlot::Lower:  return 1;
	case ESkinSlot::Weapon: return 2;
	default:                return 0;
	}
}

void AJ1CustomizePreviewActor::OnSkeletalMeshLoaded(TSoftObjectPtr<USkeletalMesh> SoftMesh, TSoftClassPtr<UAnimInstance> SoftAnim)
{
	if (USkeletalMesh* LoadedMesh = SoftMesh.Get())
	{
		GetMesh()->SetSkeletalMesh(LoadedMesh);
	}

	if (UClass* AnimClass = SoftAnim.Get())
	{
		GetMesh()->SetAnimInstanceClass(AnimClass);
	}
}

void AJ1CustomizePreviewActor::OnMaterialLoaded(TSoftObjectPtr<UMaterialInterface> SoftMat, int32 SlotIndex)
{
	if (UMaterialInterface* LoadedMat = SoftMat.Get())
	{
		GetMesh()->SetMaterial(SlotIndex, LoadedMat);
	}
}

