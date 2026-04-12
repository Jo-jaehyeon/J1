// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Login/J1LoginWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UJ1LoginWidget::NativeConstruct()
{
	if (Input_ID)
		Input_ID->OnTextCommitted.AddDynamic(this, &UJ1LoginWidget::OnInputCommitted);

	if (Input_PW)
		Input_PW->OnTextCommitted.AddDynamic(this, &UJ1LoginWidget::OnInputCommitted);

	if (Tab_Login)  Tab_Login->OnClicked.AddDynamic(this, &UJ1LoginWidget::OnTabLogin);
	if (Tab_Join)   Tab_Join->OnClicked.AddDynamic(this, &UJ1LoginWidget::OnTabJoin);

}

void UJ1LoginWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

// ════════════════════════════════════
//  위젯 바인딩 함수
// ════════════════════════════════════
void UJ1LoginWidget::OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::Type::OnEnter)
	{
		OnTabLogin();
	}
}

void UJ1LoginWidget::OnTabLogin()
{
	FString id = Input_ID->GetText().ToString();
	FString pw = Input_PW->GetText().ToString();

	Txt_Warning->SetVisibility(ESlateVisibility::Visible);
	if(id.IsEmpty())			// ID란이 비어있다면
	{
		Txt_Warning->SetText(FText::FromString(TEXT("Please input ID")));
	}
	else if(pw.IsEmpty())		// PW란이 비어있다면
	{
		Txt_Warning->SetText(FText::FromString(TEXT("Please input password")));
	}
	else
	{
		Txt_Warning->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Log, TEXT("Input id : %s, pw : %s"), *id, *pw);
		
		// TODO : Login Packet 발송
	}
}

void UJ1LoginWidget::OnTabJoin()
{
}

