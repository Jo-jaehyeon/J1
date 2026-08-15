// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/J1CreatureBase.h"
#include "InputActionValue.h"
#include "J1Player.generated.h"

UCLASS()
class J1_API AJ1Player : public AJ1CreatureBase
{
	GENERATED_BODY()

public:
	AJ1Player();
	~AJ1Player();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void Move(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);

public:
	void SetMyPlayer(bool _isMyPlayer) { isMyPlayer = _isMyPlayer; }


	/*
	*   Member Variable
	*/
protected:
	UPROPERTY(EditAnywhere, Category = "Camera")	TObjectPtr<class USpringArmComponent> CameraBoom;
	UPROPERTY(EditAnywhere, Category = "Camera")	TObjectPtr<class UCameraComponent>	  FollowCamera;

private:
	bool isMyPlayer = false;
};
