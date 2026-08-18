// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "J1CreatureBase.generated.h"

UCLASS()
class J1_API AJ1CreatureBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AJ1CreatureBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
};
