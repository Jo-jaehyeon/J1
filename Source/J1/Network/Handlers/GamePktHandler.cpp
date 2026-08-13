#include "GamePktHandler.h"

GameHandlerFunc GGamePacketHandler[UINT16_MAX];

bool Handle_Game_INVALID(SessionPtr& session, boost::asio::mutable_buffer& buffer, int32& offset)
{
	return false;
}