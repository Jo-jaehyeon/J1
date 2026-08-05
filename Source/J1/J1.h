// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Network/Packet.h"

// boost
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/buffer.hpp>


// forward declaration
class PacketSession;

using namespace boost;
using boost::asio::ip::tcp;


using SessionPtr = TSharedPtr<PacketSession>;

#define USING_SHARED_PTR(name)	using name##Ptr = TSharedPtr<class name>;


#define SEND_PACKET(sessionType, headerCode, pkt)															\
	const size_t requiredSize = PacketUtil::RequiredSize(pkt);									\
	char* rawBuffer = new char[requiredSize];													\
	auto sendBuffer = asio::buffer(rawBuffer, requiredSize);									\
	PacketUtil::Serialize(sendBuffer, headerCode, pkt);											\
	Cast<UJ1GameInstance>(GWorld->GetGameInstance())->SendPacket(sessionType, sendBuffer);