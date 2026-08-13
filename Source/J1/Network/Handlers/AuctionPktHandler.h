#pragma once
#include "../Protocol/AuctionProtocol.pb.h"
#include "Packet.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "J1.h"
#endif

bool Handle_RES_OPENAUCTION(SessionPtr& session, Game::RES_OPENAUCTION& pkt);
