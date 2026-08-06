#include "ChatPktHandler.h"
#include "J1GameInstance.h"
#include "Network/Sessions/PacketSession.h"

ChatHandlerFunc GChatPacketHandler[UINT16_MAX];

bool Handle_Chat_INVALID(SessionPtr& session, boost::asio::mutable_buffer& buffer, int32& offset)
{
	return false;
}

bool Handle_RES_ENTER_ROOM(SessionPtr& session, Chat::RES_ENTER_ROOM& pkt)
{
	bool success = pkt.result();
	int playerId = pkt.player_id();
	AsyncTask(ENamedThreads::GameThread, [success, session, playerId]() {
		if (success)
		{
			session->SetPlayerId(playerId);
			UE_LOG(LogTemp, Log, TEXT("Enter ChatRoom Success~"));
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Failed to Leave Chat Room"));
	});
	
	return true;
}

bool Handle_RES_LEAVE_ROOM(SessionPtr& session, Chat::RES_LEAVE_ROOM& pkt)
{
	bool success = pkt.result();
	AsyncTask(ENamedThreads::GameThread, [success, session]() {
		if (success)
		{
			if (auto* GI = Cast<UJ1GameInstance>(session->GetGameInstance()))
			{
				GI->Disconnect(ESessionType::Chat);
			}
			UE_LOG(LogTemp, Log, TEXT("Successfully end the chat"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to Leave Chat Room"));
		}
	});
	
	return true;
}

bool Handle_RES_CHAT(SessionPtr& session, Chat::RES_CHAT& pkt)
{
	
	FString time = FString(UTF8_TO_TCHAR(pkt.time().c_str()));
	FString sender = FString(UTF8_TO_TCHAR(pkt.sender().c_str()));
	FString msg = FString(UTF8_TO_TCHAR(pkt.message().c_str()));

	AsyncTask(ENamedThreads::GameThread, [time, sender, msg, session]() {
		if (auto* GI = Cast<UJ1GameInstance>(session->GetGameInstance()))
		{
			GI->OnChatReceived.Broadcast(time, sender, msg);
		}
	});

	return true;
}
