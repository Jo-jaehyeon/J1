#include "LobbyPktHandler.h"
#include "J1GameInstance.h"
#include "Network/Sessions/PacketSession.h"
#include "Types/J1CharacterCustomizeTypes.h"
#include "Kismet/GameplayStatics.h"

bool Handle_RES_CHECK_TOKENVALID(SessionPtr& session, Game::RES_CHECK_TOKENVALID& pkt)
{
	
	// TODO 유효하지 않은 로그인
	// 게임 강제 종료
	if (auto* GI = Cast<UJ1GameInstance>(session->GetGameInstance()))
	{
		FString temp = pkt.result() ? TEXT("정상 접근") : TEXT("비정상 접근");
		
		GI->OnNotice.Broadcast(temp);
		
	}
	
	return true;
}

bool Handle_RES_CHARACTER_LIST(SessionPtr& session, Game::RES_CHARACTER_LIST& pkt)
{
	bool result = pkt.result();

	if (result)
	{
		auto cha = pkt.characters();

		TArray<FLobbySlotInfo> Inlist;
		for (Game::LobbyCharacterInfo idx : cha)
		{
			FLobbySlotInfo temp;
			temp.CharacterUniqueID = idx.character_id();
			temp.SlotIndex = idx.slot_id();
			temp.ClassType = static_cast<ECharacterClass>(idx.classtype());
			temp.CharacterName = FString(UTF8_TO_TCHAR(idx.name().c_str()));
			temp.Level = idx.level();
			temp.UpperBodySkinRowID = idx.upperskinid();
			temp.LowerBodySkinRowID = idx.lowerskinid();
			temp.WeaponSkinRowID = idx.weaponskinid();

			Inlist.Add(temp);
		}

		AsyncTask(ENamedThreads::GameThread, [Inlist, session]() {
			if (auto* GI = Cast<UJ1GameInstance>(session->GetGameInstance()))
			{
				GI->OnLobbyListSet.Broadcast(Inlist);
			}
			});
	}

	return true;
}

bool Handle_RES_CHECK_NICKNAME(SessionPtr& session, Game::RES_CHECK_NICKNAME& pkt)
{
	bool result = pkt.result();
	AsyncTask(ENamedThreads::GameThread, [result, session]() {
		if (auto* GI = Cast<UJ1GameInstance>(session->GetGameInstance()))
		{
			GI->OnCheckNickName.Broadcast(result);
		}
	});

	return false;
}

bool Handle_RES_CREATE_CHARACTER(SessionPtr& session, Game::RES_CREATE_CHARACTER& pkt)
{
	bool result = pkt.result();

	AsyncTask(ENamedThreads::GameThread, [result, session]() {	
		if (auto* GI = Cast<UJ1GameInstance>(session->GetGameInstance()))
		{
			if (result)
			{
				UGameplayStatics::OpenLevel(GI->GetWorld(), FName("L_Lobby"));
			}
			else
			{
				FLobbySlotInfo removeCharacter;
				removeCharacter.SlotIndex = GI->GetCurrentLobbySlot();
				GI->OnLobbyListChange.Broadcast(result, removeCharacter);
				GI->OnNotice.Broadcast(TEXT("캐릭터 생성에 실패했습니다."));
				
			}
		}
	});
	
	return true;
}

bool Handle_RES_DELETE_CHARACTER(SessionPtr& session, Game::RES_DELETE_CHARACTER& pkt)
{
	return false;
}

bool Handle_RES_ENTER_GAME(SessionPtr& session, Game::RES_ENTER_GAME& pkt)
{
	return false;
}

bool Handle_RES_LEAVE_GAME(SessionPtr& session, Game::RES_LEAVE_GAME& pkt)
{
	return false;
}
