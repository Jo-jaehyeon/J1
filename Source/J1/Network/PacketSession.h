// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "J1.h"

/**
 *
 */
class J1_API PacketSession : public TSharedFromThis<PacketSession>
{
public:
	PacketSession() : _socket(nullptr) {}
	PacketSession(asio::io_context* io_context);
	~PacketSession();

	asio::io_context& GetIoContext() { return *_io_context; };

	void Run();
	void Connect(std::string host, int port);
	void Disconnect();

	void AsyncRead();

	void SendPacket(asio::mutable_buffer& buffer)
	{
		AsyncWrite(buffer);
	}

private:
	void AsyncWrite(asio::mutable_buffer& buffer);

	void OnConnect(const boost::system::error_code& err);
	void OnRead(const boost::system::error_code& err, size_t size);
	void OnWrite(const boost::system::error_code& err, size_t size);

	// PacketHeader를 읽고 패킷 구분
	void HandlePacket(char* ptr, size_t size);

private:
	asio::io_context* _io_context;
	tcp::socket _socket;
	TSharedPtr<class NetworkWorker> NetworkThread;

	static const int RecvBufferSize = 1024;
	char _recvBuffer[RecvBufferSize];
};