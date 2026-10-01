#pragma once

#include "AuctionPktHandler.h"
#include "J1GameInstance.h"
#include "Network/Sessions/PacketSession.h"

bool Handle_RES_AUCTION_LIST(SessionPtr& session, Game::RES_AUCTION_LIST& pkt)
{
	bool bmyList = pkt.mylist();
	TArray<FAuctionEntry> list;
	for (auto index : pkt.itemlist())
	{
		list.Add(FAuctionEntry{
			.ListID = index.list_id(),
			.ItemTemplateID = index.item_id(),
			.Quantity = index.count(),
			.UnitPrice = index.price(),
			.ExpireAt = UTF8_TO_TCHAR(index.expired_at().c_str())
		});
	}

	AsyncTask(ENamedThreads::GameThread, [bmyList, list, session]() {
		if (auto* GI = Cast<UJ1GameInstance>(session->GetGameInstance()))
		{
			GI->OnAuctionListSet.Broadcast(bmyList, list);
		}
		});
	return false;
}

bool Handle_RES_REGIST_ITEM(SessionPtr& session, Game::RES_REGIST_ITEM& pkt)
{
	bool result = pkt.result();

	AsyncTask(ENamedThreads::GameThread, [result, session]() {
		if (auto* GI = Cast<UJ1GameInstance>(session->GetGameInstance()))
		{
			GI->OnResponseRegister.Broadcast(result);
		}
	});
	return true;
}

bool Handle_RES_PURCHASE_ITEM(SessionPtr& session, Game::RES_PURCHASE_ITEM& pkt)
{
	return false;
}