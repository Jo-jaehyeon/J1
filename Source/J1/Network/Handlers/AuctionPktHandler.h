#pragma once
#include "../Protocol/AuctionProtocol.pb.h"
#include "Packet.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "J1.h"
#endif

bool Handle_RES_AUCTION_LIST(SessionPtr& session, Game::RES_AUCTION_LIST& pkt);
bool Handle_RES_RECEIPT_LIST(SessionPtr& session, Game::RES_RECEIPT_LIST& pkt);
bool Handle_RES_REGIST_ITEM(SessionPtr& session, Game::RES_REGIST_ITEM& pkt);
bool Handle_RES_PURCHASE_ITEM(SessionPtr& session, Game::RES_PURCHASE_ITEM& pkt);
bool Handle_RES_RECEIPT_ITEM(SessionPtr& session, Game::RES_RECEIPT_ITEM& pkt);
