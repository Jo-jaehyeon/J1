// Copyright © 2026 Jerry. All rights reserved.


#include "J1GameInstance.h"
#include "Network/Sessions/LoginSession.h"
#include "Network/Sessions/GameSession.h"
#include "Network/Sessions/ChatSession.h"

void UJ1GameInstance::Shutdown()
{
	//RequeseDisconnect();

	// 패킷을 보낼 수 있는 시간 벌기
	FPlatformProcess::Sleep(2.0f);
	
	Super::Shutdown();
}

// GameServer
void UJ1GameInstance::ConnectToServer(ESessionType sessionType)
{
	asio::io_context* io_context = new asio::io_context;

	switch (sessionType)
	{
	case ESessionType::Login: 
		loginSession = MakeShared<LoginSession>(io_context, this);
		loginSession->Connect(std::string("127.0.0.1"), 9000); 
		loginSession->Run();
		break;
	case ESessionType::Game:  
		gameSession = MakeShared<GameSession>(io_context, this);
		gameSession->Connect(std::string("127.0.0.1"), 9001);
		gameSession->Run();
		break;
	case ESessionType::Chat:  
		chatSession = MakeShared<ChatSession>(io_context, this);
		chatSession->Connect(std::string("127.0.0.1"), 9002);
		chatSession->Run();
		break;
	}
}

void UJ1GameInstance::RequestDisconnect(ESessionType sessionType)
{	
	SessionPtr targetSession = FindTargetSession(sessionType);
	if (targetSession.IsValid())
		targetSession->RequestDisconnect();
}

void UJ1GameInstance::Disconnect(ESessionType sessionType)
{
	SessionPtr targetSession = FindTargetSession(sessionType);
	if (targetSession.IsValid())
	{
		targetSession->GetIoContext().stop();
		targetSession = nullptr;
	}
	bLeaveConfirmed = true;
}

void UJ1GameInstance::SendPacket(ESessionType sessionType, asio::mutable_buffer& buffer)
{
	SessionPtr targetSession = FindTargetSession(sessionType);

	if (targetSession.IsValid())
	{
		targetSession->SendPacket(buffer);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Session is not valid!!"));
	}
}

SessionPtr UJ1GameInstance::FindTargetSession(ESessionType sessionType)
{
	SessionPtr targetSession;

	switch (sessionType)
	{
	case ESessionType::Login: targetSession = loginSession; break;
	case ESessionType::Game:  targetSession = gameSession;  break;
	case ESessionType::Chat:  targetSession = chatSession;  break;
	}

	return targetSession;
}