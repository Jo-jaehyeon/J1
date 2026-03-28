// Fill out your copyright notice in the Description page of Project Settings.


#include "J1GameInstance.h"

// GameServer
void UJ1GameInstance::ConnectToGameServer()
{
	TSharedPtr<asio::io_context> io_contextRef = MakeShared<asio::io_context>();

	// TODO : 로그인 서버 연결

	// TODO : 로그인 서버 구현 시 아래 제거
}

void UJ1GameInstance::DisconnectFromGameServer()
{	

}
