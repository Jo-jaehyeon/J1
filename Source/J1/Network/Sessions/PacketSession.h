// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "J1.h"

/**
 *
 */
class J1_API PacketSession : public TSharedFromThis<PacketSession>
{
public:
	PacketSession(asio::io_context* io_context);
	~PacketSession();

	asio::io_context& GetIoContext() { return *_io_context; };
	UGameInstance* GetGameInstance() { return GameInstance; }
	void SetPlayerId(int32 playerId) { _player_id = playerId; }

	void Run();
	void Connect(std::string host, int port);
	virtual void RequestDisconnect() = 0;

	void AsyncRead();

	void SendPacket(asio::mutable_buffer& buffer)
	{
		AsyncWrite(buffer);
	}

private:
	void AsyncWrite(asio::mutable_buffer& buffer);

	virtual void OnConnect(const boost::system::error_code& err) = 0;
	void OnRead(const boost::system::error_code& err, size_t size);
	void OnWrite(const boost::system::error_code& err, size_t size);

	// PacketHeader를 읽고 패킷 구분
	void HandlePacket(char* ptr, size_t size);

protected:
	// 자식 세션에서 목적에 맞는 핸들러에게 발송
	virtual void DispatchPacket(SessionPtr session, PacketHeader& header, char* ptr, size_t size) = 0;

protected:
	asio::io_context* _io_context;
	tcp::socket _socket;
	UGameInstance* GameInstance;
	TSharedPtr<class NetworkWorker> NetworkThread;

	int32 _player_id;

	static const int RecvBufferSize = 1024;
	char _recvBuffer[RecvBufferSize];
};