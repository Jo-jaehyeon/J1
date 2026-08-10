#include "LobbyPktHandler.h"
#include "J1GameInstance.h"
#include "Network/Sessions/PacketSession.h"
#include "Kismet/GameplayStatics.h"

bool Handle_RES_CHARACTER_LIST(SessionPtr& session, Game::RES_CHARACTER_LIST& pkt)
{
	return false;
}

bool Handle_RES_CHECK_NICKNAME(SessionPtr& session, Game::RES_CHECK_NICKNAME& pkt)
{
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
				GI->SetCurrentCharacterNum(GI->GetCurrentCharacterNum() + 1);
				UGameplayStatics::OpenLevel(GI->GetWorld(), FName("L_Lobby"));
			}
			else
			{
				FLobbySlotInfo removeCharacter;
				removeCharacter.SlotIndex = GI->GetCurrentCharacterNum() + 1;
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
