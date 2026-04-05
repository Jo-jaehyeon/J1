#include "ChatPktHandler.h"

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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Enter ChatRoom Success~")));
	
	return pkt.result();
}

bool Handle_RES_CHAT(SessionPtr& session, Chat::RES_CHAT& pkt)
{
	FString msg = FString(UTF8_TO_TCHAR(pkt.message().c_str()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, msg);

	return true;
}
