#include "LobbyPktHandler.h"
#include "Network/Sessions/PacketSession.h"

bool Handle_RES_CHARACTER_LIST(SessionPtr& session, Game::RES_CHARACTER_LIST& pkt)
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
