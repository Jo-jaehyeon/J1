#include "GamePktHandler.h"
#include "J1GameInstance.h"
#include "Network/Sessions/PacketSession.h"

GameHandlerFunc GGamePacketHandler[UINT16_MAX];

bool Handle_Game_INVALID(SessionPtr& session, boost::asio::mutable_buffer& buffer, int32& offset)
{
	return false;
}

bool Handle_RES_CHARACTER_LIST(SessionPtr& session, Game::RES_CHARACTER_LIST& pkt)
{
	bool result = pkt.result();
	FString notice = result ? TEXT("정상 접근") : TEXT("비정상 접근입니다.");
	

	AsyncTask(ENamedThreads::GameThread, [notice, session]() {
		if (auto* GI = Cast<UJ1GameInstance>(session->GetGameInstance()))
		{
			GI->OnNotice.Broadcast(notice);
		}
	});

	return true;
}

bool Handle_RES_ENTER_GAME(SessionPtr& session, Game::RES_ENTER_GAME& pkt)
{
	return false;
}

bool Handle_RES_LEAVE_GAME(SessionPtr& session, Game::RES_LEAVE_GAME& pkt)
{
	return false;
}

