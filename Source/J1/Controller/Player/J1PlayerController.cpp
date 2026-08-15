// Copyright © 2026 Jerry. All rights reserved.

#include "Controller/Player/J1PlayerController.h"
#include "Character/PC/J1Player.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Types/J1LogChannels.h"


AJ1PlayerController::AJ1PlayerController()
{
	//InputMappingContext
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_Base.IMC_Base'"));
	if (nullptr != IMCRef.Object)
		IMC_Base = IMCRef.Object;
	
	
	// InputAction
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Move.IA_Move'"));
	if (nullptr != IA_MoveRef.Object)
		IA_Move = IA_MoveRef.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_JumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Jump.IA_Jump'"));
	if (nullptr != IA_JumpRef.Object)
		IA_Jump = IA_JumpRef.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Look.IA_Look'"));
	if (nullptr != IA_LookRef.Object)
		IA_Look = IA_LookRef.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LockOnRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_LockOn.IA_LockOn'"));
	if (nullptr != IA_LockOnRef.Object)
		IA_LockOn = IA_LockOnRef.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_AttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Attack.IA_Attack'"));
	if (nullptr != IA_AttackRef.Object)
		IA_Attack = IA_AttackRef.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_SkillRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Skill.IA_Skill'"));
	if (nullptr != IA_SkillRef.Object)
		IA_Skill = IA_SkillRef.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_UIRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Inventory.IA_Inventory'"));
	if (nullptr != IA_UIRef.Object)
		IA_UI = IA_UIRef.Object;

	//Widget

}

void AJ1PlayerController::BeginPlay()
{
	Super::BeginPlay();

	ControlledCharacter = GetCharacter();
	SetInputMode(GameInputMode);

	Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		//Input Priority
		Subsystem->AddMappingContext(IMC_Base, 0);
	}
}

void AJ1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &AJ1PlayerController::JumpAct);
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AJ1PlayerController::StopJumpingAct);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AJ1PlayerController::MoveAct);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Completed, this, &AJ1PlayerController::OnMoveCompleted);
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AJ1PlayerController::LookAct);
		EnhancedInputComponent->BindAction(IA_LockOn, ETriggerEvent::Started, this, &AJ1PlayerController::ToggleLockOn);
		EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &AJ1PlayerController::AttackAct);
		EnhancedInputComponent->BindAction(IA_Skill, ETriggerEvent::Triggered, this, &AJ1PlayerController::SkillAct);
		EnhancedInputComponent->BindAction(IA_UI, ETriggerEvent::Triggered, this, &AJ1PlayerController::ShowUI);
	}
}

void AJ1PlayerController::JumpAct()
{
	if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(GetCharacter()))
	{
		ControlledPlayer->Jump();
	}
}

void AJ1PlayerController::StopJumpingAct()
{
	if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(GetCharacter()))
	{
		ControlledPlayer->StopJumping();
	}
}

void AJ1PlayerController::MoveAct(const FInputActionValue& Value)
{
	if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(GetCharacter()))
	{
		ControlledPlayer->Move(Value);
		UE_LOG(PlayerLog, Log, TEXT("%s"), *Value.ToString());
	}
}
void AJ1PlayerController::OnMoveCompleted(const FInputActionValue& Value)
{
	if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(GetCharacter()))
	{
		ControlledPlayer->Move(Value);
		UE_LOG(PlayerLog, Log, TEXT("%s"), *Value.ToString());
	}
}

void AJ1PlayerController::LookAct(const FInputActionValue& Value)
{
	if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(GetCharacter()))
	{
		ControlledPlayer->Look(Value);
	}
}

void AJ1PlayerController::ToggleLockOn()
{
	UE_LOG(LogPlayerController, Log, TEXT("Try Lock On"));

	if (bLockOnEngaged == false)
		EngagedLockOn();
	else
		DisengagedLockOn();
}

void AJ1PlayerController::EngagedLockOn()
{
}

void AJ1PlayerController::DisengagedLockOn()
{
}

void AJ1PlayerController::StopAct(const FInputActionValue& Value)
{
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->RemoveActionBinding(2);
	}
}
void AJ1PlayerController::AttackAct()
{
	if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(GetCharacter()))
	{
	}
}
void AJ1PlayerController::SkillAct(const FInputActionValue& Value)
{
	if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(GetCharacter()))
	{
	}
}
void AJ1PlayerController::ShowUI(const FInputActionValue& Value)
{
	if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(GetCharacter()))
	{
	}
}