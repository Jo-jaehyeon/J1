#pragma once
#include "../Protocol/BasicProtocol.pb.h"
#include "Packet.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "J1.h"
#endif

bool Handle_RES_SPAWN(SessionPtr& session, Game::RES_SPAWN& pkt);
bool Handle_RES_DESPAWN(SessionPtr& session, Game::RES_DESPAWN& pkt);
bool Handle_RES_MOVE(SessionPtr& session, Game::RES_MOVE& pkt);
