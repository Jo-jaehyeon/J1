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


	void Run();
	void Connect(std::string host, int port);
	void Disconnect();

private:
	void OnConnect(const boost::system::error_code& err);

private:
	asio::io_context* _io_context;
	tcp::socket _socket;
	UGameInstance* GameInstance;
};