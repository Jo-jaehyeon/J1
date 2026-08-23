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
#include "UI/Item/Inventory/J1InventoryWidget.h"
#include "UI/Item/Auction/J1AuctionWidget.h"
#include "UI/Item/Auction/J1AuctionRegisterWidget.h"
#include "UI/Item/Auction/J1AuctionPurchaseWidget.h"

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
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_UIRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_UI.IA_UI'"));
	if (nullptr != IA_UIRef.Object)
		IA_UI = IA_UIRef.Object;

	//Widget
	static ConstructorHelpers::FClassFinder<UJ1InventoryWidget> Inventory_UI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Inventory/WBP_Inventory.WBP_Inventory_C'"));
	{
		InventoryWidgetClass = Inventory_UI.Class;
	}
	static ConstructorHelpers::FClassFinder<UJ1AuctionWidget> Auction_UI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Auction/WBP_Auction.WBP_Auction_C'"));
	{
		AuctionWidgetClass = Auction_UI.Class;
	}
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

void AJ1PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	InventoryWidget = CreateWidget<UJ1InventoryWidget>(this, InventoryWidgetClass);
	AuctionWidget = CreateWidget<UJ1AuctionWidget>(this, AuctionWidgetClass);
	AuctionWidget->OnRegisterPanelOpened.AddDynamic(this, &AJ1PlayerController::OpenInventoryForAuction);
	AuctionWidget->OnReceipRequested.AddDynamic(this, &AJ1PlayerController::ReceiptAuctionProceeds);
	AuctionWidget->PurchaseWidget->OnPurchaseConfirmed.AddDynamic(this, &AJ1PlayerController::ConfirmAuctionPurchase);
	AuctionWidget->RegisterWidget->OnRegisterSubmitted.AddDynamic(this, &AJ1PlayerController::SubmitAuctionRegister);

	if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(InPawn))
	{
		if (InventoryWidget)
		{
			InventoryWidget->InitInventory(ControlledPlayer->GetInventory());
			AuctionWidget->RegisterWidget->BindToInventory(InventoryWidget);
		}
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
		UE_LOG(PlayerLog, Log, TEXT("Move : %s"), *Value.ToString());
	}
}
void AJ1PlayerController::OnMoveCompleted(const FInputActionValue& Value)
{
	if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(GetCharacter()))
	{
		ControlledPlayer->Move(Value);
		UE_LOG(PlayerLog, Log, TEXT("Move :%s"), *Value.ToString());
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
	UE_LOG(PlayerLog, Log, TEXT("UI : %s"), *Value.ToString());
	SetShowMouseCursor(true);
	if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(GetCharacter()))
	{
		SetInputMode(UIInputMode);
		SetShowMouseCursor(true);
		int index = static_cast<int>(Value.Get<float>());

		if (index == 1)
		{
			if (OpenedWidget.Contains(InventoryWidget))
			{
				OpenedWidget.RemoveSingle(InventoryWidget);
				InventoryWidget->RemoveFromParent();
			}
			else
			{
				// 인벤토리 갱신 PKT
				
				InventoryWidget->AddToViewport();
				OpenedWidget.AddUnique(InventoryWidget);
			}
		}
		else if (index == 2)
		{
			if (OpenedWidget.Contains(AuctionWidget))
			{
				OpenedWidget.RemoveSingle(AuctionWidget);
				AuctionWidget->RemoveFromParent();
			}
			else
			{
				// 옥션 갱신 PKT
				AuctionWidget->RefreshOnOpen();
				AuctionWidget->AddToViewport();
				OpenedWidget.AddUnique(AuctionWidget);
			}
		}

		if (OpenedWidget.IsEmpty())
		{
			SetInputMode(GameInputMode);
			SetShowMouseCursor(false);
		}
	}
}

//-------------------------------------------------
//					경매장 관련
//-------------------------------------------------
void AJ1PlayerController::OpenInventoryForAuction()
{
	// 인벤토리가 이미 열려있으면 중복으로 또 열 필요 없음
	if (OpenedWidget.Contains(InventoryWidget))		return; 

	InventoryWidget->AddToViewport();
	OpenedWidget.AddUnique(InventoryWidget);
}

void AJ1PlayerController::ReceiptAuctionProceeds(FAuctionEntry Entry)
{
	UE_LOG(PlayerLog, Log, TEXT("Auction claim requested. ListingID=%d"), Entry.ListID);

	// TODO: 실제 서버에 수령 요청.
	// 예) AuctionSubsystem->RequestClaim(Entry.ListingID,
	//         FOnClaimResponse::CreateUObject(this, &AJ1PlayerController::HandleClaimResponse));
	// 성공 응답이 오면 골드 지급 + AuctionWidget->RequestMyAuctionList()류를 다시 호출해서
	// 목록에서 해당 매물이 사라지도록 갱신해야 한다.
}

void AJ1PlayerController::ConfirmAuctionPurchase(FAuctionEntry Entry, int32 Quantity)
{
	UE_LOG(PlayerLog, Log, TEXT("Auction purchase requested. ListingID=%d Quantity=%d"), Entry.ListID, Quantity);

	// TODO: 실제 서버에 구매 요청. 서버가 골드 차감 + 재고 검증까지 마친 뒤,
	// 성공 응답으로 그 아이템의 실제 희귀도/스탯 데이터를 내려주면 그걸로 인벤토리에 추가해야 한다.
	// (지금 FJ1AuctionEntry엔 희귀도 정보가 없어서, 이 부분은 구조체 확장이 필요함)
	//
	// 예시(성공 응답 콜백 안에서):
	// if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(GetCharacter()))
	// {
	//     UJ1ItemInstance* NewItem = NewObject<UJ1ItemInstance>(ControlledPlayer->GetInventoryComponent());
	//     NewItem->Init(Entry.ItemTemplateID, ReceivedRarity);
	//     ControlledPlayer->GetInventoryComponent()->AddItem(NewItem, Quantity);
	// }
}


void AJ1PlayerController::SubmitAuctionRegister(UJ1InventoryManager* SourceInventory, int32 SourceSlotIndex, int64 PricePerUnit, int32 Quantity, int32 DurationHours)
{
	if (!SourceInventory)
	{
		return;
	}

	const FJ1InventorySlot Slot = SourceInventory->GetSlot(SourceSlotIndex);
	if (Slot.IsEmpty())
	{
		// 방어적 체크 - 그 사이에 아이템이 없어졌으면 실패로 바로 되돌림
		if (AuctionWidget && AuctionWidget->RegisterWidget)
		{
			AuctionWidget->RegisterWidget->HandleRegisterResult(false);
		}
		return;
	}

	UE_LOG(PlayerLog, Log, TEXT("Auction register requested. TemplateID=%d Price=%lld Qty=%d Duration=%dh"),
		Slot.ItemInstance->GetItemTemplateID(), PricePerUnit, Quantity, DurationHours);

	// TODO: 실제 서버에 등록 요청. 응답이 오면 반드시 HandleRegisterResponse(bool)를 호출해서 이어줄 것.
	// (여기서 인벤토리를 아직 건드리지 않는다 - 서버 성공 응답이 온 뒤에야 HandleRegisterResponse 안에서
	//  RegisterWidget->HandleRegisterResult(true)가 실제로 아이템을 뺀다)
}

void AJ1PlayerController::HandleAuctionListResponse(const TArray<FAuctionEntry>& Entries)
{
	if (AuctionWidget)
	{
		AuctionWidget->OnAuctionListReceived(Entries);
	}
}

void AJ1PlayerController::HandleMyAuctionListResponse(const TArray<FAuctionEntry>& Entries)
{
	if (AuctionWidget)
	{
		AuctionWidget->OnMyAuctionListReceived(Entries);
	}
}

void AJ1PlayerController::HandleRegisterResponse(bool bSuccess)
{
	if (AuctionWidget && AuctionWidget->RegisterWidget)
	{
		AuctionWidget->RegisterWidget->HandleRegisterResult(bSuccess);
	}
}

void AJ1PlayerController::HandleLowestPriceResponse(int64 LowestPrice)
{
	if (AuctionWidget && AuctionWidget->RegisterWidget)
	{
		AuctionWidget->RegisterWidget->OnLowestPriceReceived(LowestPrice);
	}
	if (AuctionWidget && AuctionWidget->PurchaseWidget)
	{
		AuctionWidget->PurchaseWidget->OnLowestPriceReceived(LowestPrice);
	}
}