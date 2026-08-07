// Copyright © 2026 Jerry. All rights reserved.


#include "UI/Lobby/J1LobbyWidget.h"
#include "Components/SizeBox.h"
#include "Components/Textblock.h"
#include "J1GameInstance.h"
#include "Protocol/GameProtocol.pb.h"

void UJ1LobbyWidget::NativeConstruct()
{
	// Binding
	UJ1GameInstance* GI = Cast<UJ1GameInstance>(GWorld->GetGameInstance());
	GI->OnNotice.AddUObject(this, &UJ1LobbyWidget::OnNotice);

	// 로그인 검증 및 캐릭터 목록 요구 패킷 발송
	Game::REQ_CHARACTER_LIST charLIST_Pkt;
	
	charLIST_Pkt.set_id(GI->GetUserid());
	charLIST_Pkt.set_token(GI->GetLoginToken());

	SEND_PACKET(GetGameInstance<UJ1GameInstance>(), ESessionType::Game, Game::PacketType::PKT_REQ_CHARACTER_LIST, charLIST_Pkt);
}

void UJ1LobbyWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UJ1LobbyWidget::OnNotice(FString text)
{

	SizeBox_Notice->SetVisibility(ESlateVisibility::Visible);
	Txt_Notice->SetText(FText::FromString(text));

	// TODO 타이머로 n초 뒤 popup 닫히게
	
}
