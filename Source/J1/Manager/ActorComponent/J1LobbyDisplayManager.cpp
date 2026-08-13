// Copyright © 2026 Jerry. All rights reserved.

#include "Manager/ActorComponent/J1LobbyDisplayManager.h"
#include "Character/PC/J1LobbyCharacterActor.h"
#include "GameFramework/Actor.h"
#include "J1GameInstance.h"
#include "Network/Protocol/LobbyProtocol.pb.h"

UJ1LobbyDisplayManager::UJ1LobbyDisplayManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UJ1LobbyDisplayManager::BeginPlay()
{
	Super::BeginPlay();
	EnsureActorPool();		// 풀링된 액터 소환

	if (UJ1GameInstance* GI = GetOwner()->GetGameInstance<UJ1GameInstance>())
	{
		GI->OnLobbyListChange.AddUObject(this, &UJ1LobbyDisplayManager::HandleChanageCharactetList);
		GI->OnLobbyListSet.AddUObject(this, &UJ1LobbyDisplayManager::SetCharacterList);
	}
}

void UJ1LobbyDisplayManager::SetCharacterList(const TArray<FLobbySlotInfo>& InList)
{
	EnsureActorPool();		// 서버 호출이 먼저 올수 있기 때문에 한번 더 호출

	CharacterList = InList;
	CharacterList.Sort([](const FLobbySlotInfo& A, const FLobbySlotInfo& B)
		{
			return A.SlotIndex < B.SlotIndex;
		});

	SelectedSlotIdx = INDEX_NONE;
	OnSelectionChanged.Broadcast(false);

	RefreshSlots();
}

void UJ1LobbyDisplayManager::SetMaxSlotCount(int32 InMaxSlotCount)
{
	TotalCharacterSlotCount = FMath::Max(0, InMaxSlotCount);
	RefreshSlots();
}

void UJ1LobbyDisplayManager::OnClickStartGame()
{
	if (const FLobbySlotInfo* Info = FindCharacterInfo(SelectedSlotIdx))
	{
		FString Name = Info->CharacterName;
		GetOwner()->GetGameInstance<UJ1GameInstance>()->OnNotice.Broadcast(Name);

		
		// TODO 게임 입장 패킷 전송
	}
}

void UJ1LobbyDisplayManager::OnClickDeleteCharacter()
{
	if (const FLobbySlotInfo* Info = FindCharacterInfoBySlotIndex(SelectedSlotIdx))
	{
		const FLobbySlotInfo InfoCopy = *Info; // RemoveCharacterLocally에서 CharacterList가 변경되므로 값 복사
		const int32 DeletedId = InfoCopy.CharacterUniqueID;

		// 캐릭 삭제 패킷 전송
		if (UJ1GameInstance* GI = GetOwner()->GetGameInstance<UJ1GameInstance>())
		{
			Game::REQ_DELETE_CHARACTER deletePkt;
			deletePkt.set_account_id(GI->GetUserid());
			deletePkt.set_character_id(DeletedId);

			SEND_PACKET(GI, ESessionType::Game, Game::PacketType::PKT_REQ_DELETE_CHARACTER, deletePkt);

			// 로비 화면에서 즉시 제거 (서버 응답을 기다리지 않는 낙관적 갱신)
			RemoveCharacterLocally(SelectedSlotIdx);
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("캐릭터 삭제 실패 : GI is not valid"));
	}
}

void UJ1LobbyDisplayManager::EnsureActorPool()
{
	UWorld* World = GetWorld();
	if (!World)												return;
	if (PooledActors.Num() > 0 || !CharacterActorClass)		return;


	const int32 PoolSize = FMath::Max(TotalCharacterSlotCount, SlotTransforms.Num());
	PooledActors.Reserve(PoolSize);

	for (int32 i = 0; i < PoolSize; ++i)
	{
		FTransform SpawnTransform = SlotTransforms.IsValidIndex(i) ? SlotTransforms[i] : FTransform::Identity;

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AJ1LobbyCharacterActor* NewActor = World->SpawnActor<AJ1LobbyCharacterActor>(CharacterActorClass, SpawnTransform, Params);
		if (NewActor)
		{
			NewActor->SetActorHiddenInGame(true);
			NewActor->OnCharacterClicked.RemoveDynamic(this, &UJ1LobbyDisplayManager::HandleCharacterClicked);
			NewActor->OnCharacterClicked.AddDynamic(this, &UJ1LobbyDisplayManager::HandleCharacterClicked);
			PooledActors.Add(NewActor);
		}
	}
}

void UJ1LobbyDisplayManager::RefreshSlots()
{
	if (PooledActors.Num() == 0)		return;
	
	for (int32 SlotIndex = 0; SlotIndex < TotalCharacterSlotCount; ++SlotIndex)
	{
		if (!PooledActors.IsValidIndex(SlotIndex))	continue;
		
		AJ1LobbyCharacterActor* SlotActor = PooledActors[SlotIndex];

		if (SlotTransforms.IsValidIndex(SlotIndex))
		{
			SlotActor->SetActorTransform(SlotTransforms[SlotIndex]);
		}
		SlotActor->SetActorHiddenInGame(false);
		
		if (const FLobbySlotInfo* Found = FindCharacterInfoBySlotIndex(SlotIndex))
		{
			// 이 슬롯 번호에 캐릭터가 있는 경우 -> 캐릭터 표시
			SlotActor->SetFilledSlot(*Found);
			SlotActor->SetSelected(Found->SlotIndex == SelectedSlotIdx);
		}
		else
		{
			// 슬롯은 있지만 캐릭터가 없는 경우 -> "캐릭터 생성" 버튼 표시
			SlotActor->SetEmptySlot(SlotIndex);
		}
	}
}

void UJ1LobbyDisplayManager::AddCharacterLocally(FLobbySlotInfo& Info)
{
	CharacterList.Add(Info);
}

void UJ1LobbyDisplayManager::RemoveCharacterLocally(const int32& slotIdx)
{
	CharacterList.RemoveAll([&slotIdx](const FLobbySlotInfo& C)
		{
			return C.SlotIndex == slotIdx;
		});

	if (SelectedSlotIdx == slotIdx)
	{
		SelectedSlotIdx = INDEX_NONE;
		OnSelectionChanged.Broadcast(false);
	}

	// 삭제된 캐릭터 액터를 즉시 숨기고, 뒤 페이지의 캐릭터를 당겨와 재배치
	RefreshSlots();
}

void UJ1LobbyDisplayManager::HandleChanageCharactetList(bool Isadd, FLobbySlotInfo& Info)
{
	if (Isadd)
		AddCharacterLocally(Info);
	else
		RemoveCharacterLocally(Info.SlotIndex);
}

FLobbySlotInfo* UJ1LobbyDisplayManager::FindCharacterInfo(int32 CharacterId)
{
	return CharacterList.FindByPredicate([CharacterId](const FLobbySlotInfo& C)
		{
			return C.CharacterUniqueID == CharacterId;
		});
}

const FLobbySlotInfo* UJ1LobbyDisplayManager::FindCharacterInfoBySlotIndex(int32 InSlotIndex) const
{
	return CharacterList.FindByPredicate([InSlotIndex](const FLobbySlotInfo& C)
	{
		return C.SlotIndex == InSlotIndex;
	});
}

void UJ1LobbyDisplayManager::HandleCharacterClicked(int32 ClickedSlotId)
{
	SelectedSlotIdx = ClickedSlotId;

	// 현재 페이지에 보이는 모든 액터의 선택 표시를 갱신
	for (AJ1LobbyCharacterActor* SlotActor : PooledActors)
	{
		if (SlotActor && !SlotActor->IsHidden())
		{
			SlotActor->SetSelected(SlotActor->GetCharacterSlotIdx() == SelectedSlotIdx);
		}
	}
	if (UJ1GameInstance* GI = Cast<UJ1GameInstance>(GetOwner()->GetGameInstance()))
		GI->SetCurrentLobbySlot(SelectedSlotIdx);

	OnSelectionChanged.Broadcast(HasValidSelection());
}