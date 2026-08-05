// Copyright © 2026 Jerry. All rights reserved.

#include "UI/Chat/J1ChatWidget.h"
#include "Components/ScrollBox.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "J1ChatEntryWidget.h"
#include "Protocol/ChatProtocol.pb.h"
#include "J1GameInstance.h"

void UJ1ChatWidget::NativeConstruct()
{
	if(EditableText_Input)
		EditableText_Input->OnTextCommitted.AddDynamic(this, &UJ1ChatWidget::OnInputTextCommitted);

	if (Tab_All)     Tab_All->OnClicked.AddDynamic(this, &UJ1ChatWidget::OnTabAll);
	if (Tab_Party)   Tab_Party->OnClicked.AddDynamic(this, &UJ1ChatWidget::OnTabParty);
	if (Tab_Guild)   Tab_Guild->OnClicked.AddDynamic(this, &UJ1ChatWidget::OnTabGuild);
	if (Tab_Whisper) Tab_Whisper->OnClicked.AddDynamic(this, &UJ1ChatWidget::OnTabWhisper);
	if (Tab_System)  Tab_System->OnClicked.AddDynamic(this, &UJ1ChatWidget::OnTabSystem);


	// Binding
	UJ1GameInstance* GI = Cast<UJ1GameInstance>(GWorld->GetGameInstance());
	GI->OnChatReceived.AddUObject(this, &UJ1ChatWidget::OnChatReceived);
}

void UJ1ChatWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

// ════════════════════════════════════
//  블루프린트 호출 함수
// ════════════════════════════════════
void UJ1ChatWidget::SetActiveChannel(EChatChannel Channel)
{
}

// ════════════════════════════════════
//  위젯 바인딩 함수
// ════════════════════════════════════
void UJ1ChatWidget::OnTabAll() { SetActiveChannel(EChatChannel::All); }
void UJ1ChatWidget::OnTabSystem() { SetActiveChannel(EChatChannel::System); }
void UJ1ChatWidget::OnTabWhisper() { SetActiveChannel(EChatChannel::Whisper); }
void UJ1ChatWidget::OnTabParty() { SetActiveChannel(EChatChannel::Party); }
void UJ1ChatWidget::OnTabGuild() { SetActiveChannel(EChatChannel::Guild); }


void UJ1ChatWidget::OnInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::Type::OnEnter)
	{
		UE_LOG(LogTemp, Log, TEXT("%s"), *Text.ToString());
		EditableText_Input->SetText(FText::GetEmpty());
		SendText(Text);
	}
}


void UJ1ChatWidget::SendText(const FText& Text)
{
	Chat::REQ_CHAT ChatPkt;

	FDateTime Now = FDateTime::Now();
	FString Time = FString::Printf(TEXT("%02d:%02d"),
		Now.GetHour(),
		Now.GetMinute());
	std::string str_Time = std::string(TCHAR_TO_UTF8(*Time));
	ChatPkt.set_time(str_Time);

	ChatPkt.set_sender("admin");

	FString fstr_msg = Text.ToString();
	std::string str_msg = std::string(TCHAR_TO_UTF8(*fstr_msg));
	ChatPkt.set_message(str_msg);

	SEND_PACKET(Chat::PacketType::PKT_REQ_CHAT, ChatPkt);
}

// ════════════════════════════════════
//  채팅 관련 함수
// ════════════════════════════════════
void UJ1ChatWidget::OnChatReceived(const FString& Time, const FString& Sender, const FString& Message)
{
	if (!ScrollBox_ChatLog || !ChatEntryWidgetClass) return;

	UJ1ChatEntryWidget* Entry = CreateWidget<UJ1ChatEntryWidget>(GetOwningPlayer(), ChatEntryWidgetClass);
	if (!Entry)		return;


	// 발신 시간 설정
	if (UTextBlock* TimeTB = Cast<UTextBlock>(Entry->GetWidgetFromName(TEXT("Txt_Time"))))
		TimeTB->SetText(FText::FromString(Time));

	// 발신자 이름 설정
	if (UTextBlock* SenderTB = Cast<UTextBlock>(Entry->GetWidgetFromName(TEXT("Txt_Sender"))))
	{
		SenderTB->SetText(FText::FromString(Sender));
	}

	// 내용 설정
	if (UTextBlock* ContentTB = Cast<UTextBlock>(Entry->GetWidgetFromName(TEXT("Txt_Content"))))
	{
		ContentTB->SetText(FText::FromString(Message));
	}


	ScrollBox_ChatLog->AddChild(Entry);
	ScrollBox_ChatLog->ScrollToEnd();
}
