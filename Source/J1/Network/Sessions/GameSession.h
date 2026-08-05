// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "J1.h"
#include "Network/Sessions/PacketSession.h"

/**
 *
 */
class J1_API GameSession : public PacketSession
{
public:
	GameSession(asio::io_context* io_context);
	~GameSession();


private:
	virtual void RequestDisconnect() override;
	virtual void OnConnect(const boost::system::error_code& err) override;
	
	// PacketHeader를 읽고 패킷 구분
	virtual void DispatchPacket(SessionPtr session, PacketHeader& header, char* ptr, size_t size) override;
};