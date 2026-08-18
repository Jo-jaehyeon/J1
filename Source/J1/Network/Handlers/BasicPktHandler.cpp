#include "BasicPktHandler.h"
#include "J1GameInstance.h"
#include "Network/Sessions/PacketSession.h"

bool Handle_RES_SPAWN(SessionPtr& session, Game::RES_SPAWN& pkt)
{
	return true;
}

bool Handle_RES_DESPAWN(SessionPtr& session, Game::RES_DESPAWN& pkt)
{
	return true;
}

bool Handle_RES_MOVE(SessionPtr& session, Game::RES_MOVE& pkt)
{
	return true;
}
