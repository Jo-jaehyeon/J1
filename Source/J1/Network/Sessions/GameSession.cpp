#include "GameSession.h"
#include "NetworkWorker.h"
#include "Handlers/GamePktHandler.h"
#include "../../J1GameInstance.h"

GameSession::GameSession(asio::io_context* io_context, UJ1GameInstance* gameInstance)
	: PacketSession(io_context, gameInstance)
{
	GamePktHandler::Init();
}

GameSession::~GameSession()
{
}

void GameSession::RequestDisconnect()
{
	// Leave Room Pkt 발송
	//Game::REQ_LEAVE_ROOM pkt;
	//pkt.set_player_id(_player_id);
	//
	//SEND_PACKET(Game::PacketType::PKT_REQ_LEAVE_ROOM, pkt);
	PacketSession::Disconnect();
}

void GameSession::OnConnect(const boost::system::error_code& err)
{
	if (!err)
	{
		UJ1GameInstance* GI = GetGameInstance();
		Game::REQ_CHECK_TOKENVALID checkPkt;
		checkPkt.set_id(GI->GetUserid());
		checkPkt.set_token(GI->GetLoginToken());
		
		SEND_PACKET(GI, ESessionType::Game, Game::PacketType::PKT_REQ_CHECK_TOKENVALID, checkPkt);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Connection Success~")));
		
		AsyncRead();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connect Failed")));
	}
}

void GameSession::DispatchPacket(SessionPtr session, PacketHeader& header, char* ptr, size_t size)
{
	GamePktHandler::HandlePacket(session, header, ptr, size);
}
