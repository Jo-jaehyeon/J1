// Fill out your copyright notice in the Description page of Project Settings.


#include "J1GameInstance.h"
#include "Network/J1PacketSession.h"

using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

// GameServer
void UJ1GameInstance::ConnectToGameServer()
{
	asio::io_context* io_context = new asio::io_context;

	// TODO : 로그인 서버 연결

	// TODO : 로그인 서버 구현 시 아래 제거
	GameSession = MakeShared<PacketSession>(io_context);

	GameSession->Connect(std::string("127.0.0.1"), 4242);
	
	// TODO : Thread 작업 추가
	// io_context 리턴 방지를 위한 임시 코드
	// gameinstance에 추가하다 보니 게임이 멈춰버림
	work_guard_type work_guard(io_context->get_executor());
	io_context->run();

}

void UJ1GameInstance::DisconnectFromGameServer()
{	

}
