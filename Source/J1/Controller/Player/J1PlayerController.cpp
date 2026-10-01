// Copyright © 2026 Jerry. All rights reserved.

#include "Controller/Player/J1PlayerController.h"
#include "Character/PC/J1Player.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "J1GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Types/J1LogChannels.h"
#include "UI/Item/Inventory/J1InventoryWidget.h"
#include "UI/Item/Auction/J1AuctionWidget.h"
#include "UI/Item/Auction/J1AuctionRegisterWidget.h"
#include "UI/Item/Auction/J1AuctionPurchaseWidget.h"
#include "Manager/ActorComponent/J1InventoryManager.h"
#include "Item/J1ItemInstance.h"
#include "Network/Protocol/AuctionProtocol.pb.h"

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

	if (UJ1GameInstance* GI = Cast<UJ1GameInstance>(GetGameInstance()))
	{
		SetPlayerId(GI->GetUserid());
		GI->OnAuctionListSet.AddUObject(this, &AJ1PlayerController::HandleAuctionListResponse);
		GI->OnResponseRegister.AddUObject(this, &AJ1PlayerController::HandleRegisterResponse);
		GI->OnResponseLowestPrice.AddUObject(this, &AJ1PlayerController::HandleLowestPriceResponse);
		GI->OnAuctionReceiptListSet.AddUObject(this, &AJ1PlayerController::HandleReceiptListResponse);
		GI->OnResponseReceipt.AddUObject(this, &AJ1PlayerController::HandleReceiptResponse);
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
	AuctionWidget->OnRequestList.AddDynamic(this, &AJ1PlayerController::RequestAuctionList);
	AuctionWidget->OnRequestReceiptList.AddDynamic(this, &AJ1PlayerController::RequestAuctionReceiptList);
	AuctionWidget->OnReceiptRequested.AddDynamic(this, &AJ1PlayerController::RequestAuctionReceipt);
	AuctionWidget->PurchaseWidget->OnPurchaseConfirmed.AddDynamic(this, &AJ1PlayerController::ConfirmAuctionPurchase);
	AuctionWidget->RegisterWidget->OnRegisterSubmitted.AddDynamic(this, &AJ1PlayerController::SubmitAuctionRegister);

	if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(InPawn))
	{
		// 캐릭터 선택 시 GameInstance에 저장해 둔 DB 골드로 인벤토리 골드 초기화
		if (UJ1InventoryManager* Inventory = ControlledPlayer->GetInventory())
		{
			if (UJ1GameInstance* GI = Cast<UJ1GameInstance>(GetGameInstance()))
			{
				Inventory->SetGold(GI->GetCharacterInfo().Gold);
			}
		}

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

void AJ1PlayerController::RequestAuctionList(bool bmyList, FString searchName)
{
	Game::REQ_AUCTION_LIST listPkt;
	listPkt.set_player_id(player_id);
	listPkt.set_mylist(bmyList);
	listPkt.set_search_item(TCHAR_TO_UTF8(*searchName));

	SEND_PACKET(GetGameInstance<UJ1GameInstance>(), ESessionType::Game, Game::PKT_REQ_AUCTION_LIST, listPkt);
}

void AJ1PlayerController::RequestAuctionReceiptList()
{
	// TODO: 수령함 목록 요청 패킷 (AuctionProtocol에 아직 없음 - REQ/RES_AUCTION_RECEIPT_LIST 추가 필요)
	// 예)
	// Game::REQ_AUCTION_RECEIPT_LIST receiptListPkt;
	// receiptListPkt.set_player_id(player_id);
	// SEND_PACKET(GetGameInstance<UJ1GameInstance>(), ESessionType::Game, Game::PKT_REQ_AUCTION_RECEIPT_LIST, receiptListPkt);
	// 응답 핸들러에서 GI->OnAuctionReceiptListSet.Broadcast(Receipts) 호출.
}

void AJ1PlayerController::RequestAuctionReceipt(FAuctionReceiptEntry ReceiptEntry)
{
	if (!ReceiptEntry.IsValid() || PendingReceipts.Contains(ReceiptEntry.ReceiptID))		return;

	UE_LOG(PlayerLog, Log, TEXT("Auction receipt requested. ReceiptID=%lld Type=%d"), ReceiptEntry.ReceiptID, static_cast<int32>(ReceiptEntry.ReceiptType));

	PendingReceipts.Add(ReceiptEntry.ReceiptID, ReceiptEntry);

	// TODO: 수령 요청 패킷 (AuctionProtocol에 아직 없음 - REQ/RES_AUCTION_RECEIPT 추가 필요)
	// 예)
	// Game::REQ_AUCTION_RECEIPT receiptPkt;
	// receiptPkt.set_player_id(player_id);
	// receiptPkt.set_receipt_id(ReceiptEntry.ReceiptID);
	// SEND_PACKET(GetGameInstance<UJ1GameInstance>(), ESessionType::Game, Game::PKT_REQ_AUCTION_RECEIPT, receiptPkt);
	// 응답 핸들러에서 GI->OnResponseReceipt.Broadcast(ReceiptID, bSuccess) 호출.
}

void AJ1PlayerController::ConfirmAuctionPurchase(FAuctionEntry Entry, int32 Quantity)
{
	UE_LOG(PlayerLog, Log, TEXT("Auction purchase requested. ListingID=%lld Quantity=%d"), Entry.ListID, Quantity);

	// TODO: 실제 서버에 구매 요청 (REQ_PURCHASE_ITEM).
	// 구매한 아이템은 즉시 인벤토리에 넣지 않는다. 서버가 골드 차감 + 재고 검증 후
	// auction_receipts에 PurchasedItem 행을 만들고, 플레이어는 수령함 탭에서 받아간다.
}


void AJ1PlayerController::SubmitAuctionRegister(UJ1InventoryManager* SourceInventory, int32 SourceSlotIndex, int64 PricePerUnit, int32 Quantity, FString DurationHours)
{
	if (!SourceInventory)		return;
	
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

	UE_LOG(PlayerLog, Log, TEXT("Auction register requested. TemplateID=%d Price=%lld Qty=%d Duration=%s"),
		Slot.ItemInstance->GetItemTemplateID(), PricePerUnit, Quantity, *DurationHours);

	Game::REQ_REGIST_ITEM registPkt;
	Game::AuctionItemInfo* temp = registPkt.add_registinfo();
	temp->set_player_id(player_id);
	temp->set_item_id(Slot.ItemInstance->GetItemTemplateID());
	temp->set_count(Quantity);
	temp->set_price(PricePerUnit);

	std::string t = TCHAR_TO_UTF8(*DurationHours);
	temp->set_expired_at(t);

	SEND_PACKET(GetGameInstance<UJ1GameInstance>(), ESessionType::Game, Game::PKT_REQ_REGIST_ITEM, registPkt);
}

void AJ1PlayerController::HandleAuctionListResponse(bool myList, const TArray<FAuctionEntry>& Entries)
{
	if (AuctionWidget)
	{
		if(myList)	AuctionWidget->OnMyAuctionListReceived(Entries);
		else        AuctionWidget->OnAuctionListReceived(Entries);
	}
}

void AJ1PlayerController::HandleRegisterResponse(bool bSuccess)
{
	if (AuctionWidget && AuctionWidget->RegisterWidget)
	{
		AuctionWidget->RegisterWidget->HandleRegisterResult(bSuccess);
	}

	// 등록 성공 시 내 등록 물품 목록 갱신
	if (bSuccess && AuctionWidget)
	{
		AuctionWidget->RefreshMyList();
	}
}

void AJ1PlayerController::HandleReceiptListResponse(const TArray<FAuctionReceiptEntry>& Receipts)
{
	if (AuctionWidget)
	{
		AuctionWidget->OnReceiptListReceived(Receipts);
	}
}

void AJ1PlayerController::HandleReceiptResponse(int64 ReceiptID, bool bSuccess)
{
	FAuctionReceiptEntry ReceiptEntry;
	if (!PendingReceipts.RemoveAndCopyValue(ReceiptID, ReceiptEntry))		return;

	// 확정 갱신 : 서버 성공 응답을 받은 뒤에만 실제로 지급한다.
	if (bSuccess)
	{
		if (ReceiptEntry.IsGoldReceipt())
		{
			if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(GetCharacter()))
			{
				if (UJ1InventoryManager* Inventory = ControlledPlayer->GetInventory())
				{
					Inventory->AddGold(ReceiptEntry.GoldAmount);
				}
			}
		}
		else if (AJ1Player* ControlledPlayer = Cast<AJ1Player>(GetCharacter()))
		{
			if (UJ1InventoryManager* Inventory = ControlledPlayer->GetInventory())
			{
				// TODO: 장비 희귀도/스탯은 현재 경매 데이터에 없어서 기본값으로 생성.
				// 서버가 희귀도를 내려주게 되면 FAuctionReceiptEntry에 필드를 추가해서 넘길 것.
				UJ1ItemInstance* NewItem = NewObject<UJ1ItemInstance>(Inventory);
				NewItem->Init(static_cast<int32>(ReceiptEntry.ItemTemplateID), EItemRarity::Common);
				const int32 Remaining = Inventory->AddItem(NewItem, static_cast<int32>(ReceiptEntry.Quantity));
				if (Remaining > 0)
				{
					// TODO: 서버는 이미 수령 처리(행 삭제)를 끝낸 상태라 남은 수량이 유실된다.
					// 요청 전에 인벤토리 여유 공간을 검사하거나, 서버가 공간 검사 후 응답하도록 바꿀 것.
					UE_LOG(PlayerLog, Warning, TEXT("Auction receipt overflow. TemplateID=%lld Remaining=%d"), ReceiptEntry.ItemTemplateID, Remaining);
				}
			}
		}
	}

	if (AuctionWidget)
	{
		AuctionWidget->HandleReceiptResult(ReceiptID, bSuccess);
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