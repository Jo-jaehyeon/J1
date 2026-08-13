// Copyright © 2026 Jerry. All rights reserved.


#include "UI/Lobby/J1LobbyWidget.h"
#include "J1GameInstance.h"
#include "Components/Overlay.h"
#include "Components/Textblock.h"
#include "Components/Button.h"
#include "Manager/ActorComponent/J1LobbyDisplayManager.h"
#include "Kismet/GameplayStatics.h"
#include "Network/Protocol/LobbyProtocol.pb.h"

void UJ1LobbyWidget::NativeConstruct()
{
	// Binding
	UJ1GameInstance* GI = Cast<UJ1GameInstance>(GWorld->GetGameInstance());
	GI->OnNotice.AddUObject(this, &UJ1LobbyWidget::OnNotice);

	if (Btn_StartGame)			Btn_StartGame->OnClicked.AddDynamic(this, &UJ1LobbyWidget::HandleStartGameClicked);
	if (Btn_DeleteCharacter)	Btn_DeleteCharacter->OnClicked.AddDynamic(this, &UJ1LobbyWidget::HandleDeleteCharacterClicked);
	
	// 선택된 캐릭터가 없을 때는 두 버튼 모두 비활성화
	HandleSelectionChanged(false);	

	// 캐릭터 목록 요구 패킷 발송
	Game::REQ_CHARACTER_LIST charLIST_Pkt;
	
	charLIST_Pkt.set_id(GI->GetUserid()); 

	SEND_PACKET(GetGameInstance<UJ1GameInstance>(), ESessionType::Game, Game::PacketType::PKT_REQ_CHARACTER_LIST, charLIST_Pkt);
}

void UJ1LobbyWidget::NativeDestruct()
{
	if (DisplayManager)
	{
		DisplayManager->OnSelectionChanged.RemoveDynamic(this, &UJ1LobbyWidget::HandleSelectionChanged);
	}
	Super::NativeDestruct();
}

void UJ1LobbyWidget::SetDisplayManager(UJ1LobbyDisplayManager* InManager)
{
	if (DisplayManager)
	{
		DisplayManager->OnSelectionChanged.RemoveDynamic(this, &UJ1LobbyWidget::HandleSelectionChanged);
	}

	DisplayManager = InManager;
	if (DisplayManager)
	{
		DisplayManager->OnSelectionChanged.AddDynamic(this, &UJ1LobbyWidget::HandleSelectionChanged);
		HandleSelectionChanged(DisplayManager->HasValidSelection());
	}
}

void UJ1LobbyWidget::HandleStartGameClicked()
{
	if (DisplayManager)
	{
		DisplayManager->OnClickStartGame();
	}
}

void UJ1LobbyWidget::HandleDeleteCharacterClicked()
{
	if (DisplayManager)
	{
		// 매니저 내부에서 서버 요청 브로드캐스트 + 로비 화면 즉시 제거(RemoveCharacterLocally)를 함께 수행한다.
		DisplayManager->OnClickDeleteCharacter();
	}
}

void UJ1LobbyWidget::HandleSelectionChanged(bool bHasValidSelection)
{
	if (Btn_StartGame)			Btn_StartGame->SetIsEnabled(bHasValidSelection);
	if (Btn_DeleteCharacter)	Btn_DeleteCharacter->SetIsEnabled(bHasValidSelection);
}

void UJ1LobbyWidget::OnNotice(FString text)
{
	Overlay_Notice->SetVisibility(ESlateVisibility::Visible);
	Txt_Notice->SetText(FText::FromString(text));

	TWeakObjectPtr<UOverlay> WeakOverlay(Overlay_Notice);

	GetWorld()->GetTimerManager().ClearTimer(PopupVisibilityTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(
		PopupVisibilityTimerHandle,
		FTimerDelegate::CreateLambda([WeakOverlay]()
			{
				if (WeakOverlay.IsValid())
				{
					WeakOverlay->SetVisibility(ESlateVisibility::Hidden);
				}
			}),
		1.0f,
		false
	);
}
