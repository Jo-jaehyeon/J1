// Copyright © 2026 Jerry. All rights reserved.

#include "Character/J1CreatureBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AJ1CreatureBase::AJ1CreatureBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AJ1CreatureBase::BeginPlay()
{
	Super::BeginPlay();
}

void AJ1CreatureBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AJ1CreatureBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AJ1CreatureBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!IsPlayerControlled())
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);  // 회전 속도

		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}
}
