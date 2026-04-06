// Fill out your copyright notice in the Description page of Project Settings.


#include "J1GameInstance.h"
#include "PacketSession.h"

void UJ1GameInstance::Shutdown()
{
	RequeseDisconnect();

	// 패킷을 보낼 수 있는 시간 벌기
	FPlatformProcess::Sleep(2.0f);
	
	Super::Shutdown();
}

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

void UJ1GameInstance::RequeseDisconnect()
{	
	GameSession->RequestDisconnect();
}

void UJ1GameInstance::Disconnect()
{
	GameSession->GetIoContext().stop();
	GameSession = nullptr;
	bLeaveConfirmed = true;
}

void UJ1GameInstance::SendPacket(asio::mutable_buffer& buffer)
{
	if (GameSession.IsValid())
	{
		GameSession->SendPacket(buffer);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameSession is not valid!!"));
	}
}