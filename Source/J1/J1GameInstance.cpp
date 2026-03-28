// Fill out your copyright notice in the Description page of Project Settings.


#include "J1GameInstance.h"
#include "PacketSession.h"

// GameServer
void UJ1GameInstance::ConnectToGameServer()
{
	asio::io_context* io_context = new asio::io_context;

	// TODO : 로그인 서버 연결

	// TODO : 로그인 서버 구현 시 아래 제거
	GameSession = MakeShared<PacketSession>(io_context);

	GameSession->Connect(std::string("127.0.0.1"), 4242);
	GameSession->Run();
	
}

void UJ1GameInstance::DisconnectFromGameServer()
{	
	GameSession->GetIoContext().stop();
	GameSession = nullptr;
}
