// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Chat/J1ChatWidget.h"
#include "Components/ScrollBox.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void UJ1ChatWidget::NativeConstruct()
{
	if(EditableText_Input)
		EditableText_Input->OnTextCommitted.AddDynamic(this, &UJ1ChatWidget::OnInputTextCommitted);

	if (Tab_All)     Tab_All->OnClicked.AddDynamic(this, &UJ1ChatWidget::OnTabAll);
	if (Tab_Party)   Tab_Party->OnClicked.AddDynamic(this, &UJ1ChatWidget::OnTabParty);
	if (Tab_Guild)   Tab_Guild->OnClicked.AddDynamic(this, &UJ1ChatWidget::OnTabGuild);
	if (Tab_Whisper) Tab_Whisper->OnClicked.AddDynamic(this, &UJ1ChatWidget::OnTabWhisper);
	if (Tab_System)  Tab_System->OnClicked.AddDynamic(this, &UJ1ChatWidget::OnTabSystem);
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
	UE_LOG(LogTemp, Log, TEXT("%s"), *Text.ToString());
	EditableText_Input->SetText(FText::GetEmpty());
}
