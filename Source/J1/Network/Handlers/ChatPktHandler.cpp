#include "ChatPktHandler.h"
#include "J1GameInstance.h"
#include "PacketSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(SessionPtr& session, boost::asio::mutable_buffer& buffer, int32& offset)
{
	return false;
}

bool Handle_RES_ENTER_ROOM(SessionPtr& session, Chat::RES_ENTER_ROOM& pkt)
{
	if(pkt.result())
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Enter ChatRoom Success~")));
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Enter ChatRoom Failed~")));
	
	return pkt.result();
}

bool Handle_RES_LEAVE_ROOM(SessionPtr& session, Chat::RES_LEAVE_ROOM& pkt)
{
	bool success = pkt.result();
	if (success)
	{
		AsyncTask(ENamedThreads::GameThread, [session]() {
			if (auto* GI = Cast<UJ1GameInstance>(session->GetGameInstance()))
			{
				GI->Disconnect();
			}
			});
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Leave Chat Room"));
	}

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
