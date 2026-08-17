// Copyright © 2026 Jerry. All rights reserved.

#include "Character/PC/J1Player.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

AJ1Player::AJ1Player() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	//Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//Capsule (프로파일 설정)
	//GetCapsuleComponent()->SetCollisionProfileName();

	//Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.7f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// CameraBoom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->TargetArmLength = 900.f;
	CameraBoom->SetRelativeRotation(FRotator(-25.f, 0.f, 0.f));
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;

	// Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	
	//Inventory
	MyInventory = CreateDefaultSubobject<UJ1InventoryManager>(TEXT("MyInventory"));
}

AJ1Player::~AJ1Player()
{
}

void AJ1Player::BeginPlay()
{
	Super::BeginPlay();
}

void AJ1Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AJ1Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AJ1Player::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator CameraRotation = FollowCamera->GetComponentRotation();

	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FRotator CameraYawRotation(0, CameraRotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(CameraYawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);

	//if (isMyPlayer)
	//{
	//	SetMoveState(message::MOVE_STATE_RUN);
	//	DesiredInput = MovementVector;
	//
	//	DesiredMoveDirection = FVector::ZeroVector;
	//	DesiredMoveDirection += ForwardDirection * MovementVector.Y;
	//	DesiredMoveDirection += RightDirection * MovementVector.X;
	//	DesiredMoveDirection.Normalize();
	//
	//	const FVector Location = GetActorLocation();
	//
	//	FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Location, Location + DesiredMoveDirection);
	//	DesiredYaw = Rotator.Yaw;
	//}
}

void AJ1Player::Look(const FInputActionValue& Value)
{
	
}

