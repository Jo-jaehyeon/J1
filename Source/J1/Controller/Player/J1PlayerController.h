// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "J1PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class J1_API AJ1PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AJ1PlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	// Lock On
	UFUNCTION(BlueprintCallable) void ToggleLockOn();
	UFUNCTION(BlueprintCallable) void EngagedLockOn();
	UFUNCTION(BlueprintCallable) void DisengagedLockOn();

protected:
	void MoveAct(const FInputActionValue& Value);
	void OnMoveCompleted(const FInputActionValue& Value);
	void JumpAct();
	void StopJumpingAct();
	void LookAct(const FInputActionValue& Value);
	void StopAct(const FInputActionValue& Value);
	void AttackAct();
	void SkillAct(const FInputActionValue& Value);
	void ShowUI(const FInputActionValue& Value);

	/*
	*  Member Variable
	*/
public:
	UPROPERTY(BlueprintReadWrite) bool bLockOnEngaged;
	UPROPERTY(BlueprintReadWrite) bool bShouldRotate;

protected:
	UPROPERTY(EditAnywhere)
	ACharacter* ControlledCharacter;

	//InputMappingContext
	TObjectPtr <class UEnhancedInputLocalPlayerSubsystem> Subsystem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> IMC_Base;

	//Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_Move;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_Jump;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_Look;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_LockOn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_Attack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_Skill;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_UI;

	// Widget


private:
	FInputModeGameOnly GameInputMode;
	FInputModeGameAndUI UIInputMode;

};
