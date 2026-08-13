// Copyright © 2026 Jerry. All rights reserved.

#include "UI/Login/J1LoginWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Protocol/LoginProtocol.pb.h"
#include "Kismet/GameplayStatics.h"
#include "J1GameInstance.h"

void UJ1LoginWidget::NativeConstruct()
{
	if (Input_ID)
		Input_ID->OnTextCommitted.AddDynamic(this, &UJ1LoginWidget::OnInputCommitted);

	if (Input_PW)
		Input_PW->OnTextCommitted.AddDynamic(this, &UJ1LoginWidget::OnInputCommitted);

	if (Tab_Login)		Tab_Login->OnClicked.AddDynamic(this, &UJ1LoginWidget::OnTabLogin);
	if (Tab_Join)		Tab_Join->OnClicked.AddDynamic(this, &UJ1LoginWidget::OnTabJoin);
	if (Tab_CheckID)	Tab_CheckID->OnClicked.AddDynamic(this, &UJ1LoginWidget::OnTabCheckID);

	// Binding
	UJ1GameInstance* GI = Cast<UJ1GameInstance>(GWorld->GetGameInstance());
	GI->OnLoginResult.AddUObject(this, &UJ1LoginWidget::OnResultLogin);

}

void UJ1LoginWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

// ═════════════════════
//  위젯 바인딩 함수
// ═════════════════════
void UJ1LoginWidget::OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::Type::OnEnter)
	{
		if(bLoginMode)
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
		
		TryLogin(id, pw);
	}
}

void UJ1LoginWidget::OnTabJoin()
{
	if (bLoginMode)		// 로그인 모드에서 클릭됐다면 회원가입 모드로 전환
	{
		SwitchAuthMode();
		bLoginMode = false;
	}
	else                // 회원가입 모드에서 클릭됐다면 회원가입 시도
	{
		// 회원가입 가능한지 체크
		FString id =		 Input_ID->GetText().ToString();
		FString pw =		 Input_PW->GetText().ToString();
		FString checked_pw = Input_CheckPW->GetText().ToString();

		if (pw == checked_pw)		TrySignUp(id, pw);
		else
		{
			Txt_Warning->SetVisibility(ESlateVisibility::Visible);
			Txt_Warning->SetText(FText::FromString(TEXT("비밀번호를 확인해주세요")));
		}
	}
}

void UJ1LoginWidget::OnTabCheckID()
{
	Txt_Warning->SetVisibility(ESlateVisibility::Visible);

	FString id = Input_ID->GetText().ToString();
	if (id.IsEmpty())			// ID란이 비어있다면
	{
		Txt_Warning->SetText(FText::FromString(TEXT("Please input ID")));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("check id : %s"), *id);
		TryCheckID(id);
	}
}

// ═════════════════════
//      패킷 발송
// ═════════════════════
void UJ1LoginWidget::TryLogin(FString input_id, FString input_pw)
{
	Login::REQ_LOGIN LoginPkt;

	std::string idString = TCHAR_TO_UTF8(*input_id);
	std::string pwString = TCHAR_TO_UTF8(*input_pw);
	LoginPkt.set_id(idString);
	LoginPkt.set_pw(pwString);
	
	SEND_PACKET(GetGameInstance<UJ1GameInstance>(), ESessionType::Login, Login::PacketType::PKT_REQ_LOGIN, LoginPkt);
}

void UJ1LoginWidget::TryCheckID(FString input_id)
{
	Login::REQ_CHECK_ID CheckPkt;

	std::string idString = TCHAR_TO_UTF8(*input_id);
	CheckPkt.set_id(idString);

	SEND_PACKET(GetGameInstance<UJ1GameInstance>(), ESessionType::Login, Login::PacketType::PKT_REQ_CHECK_ID, CheckPkt);
}

void UJ1LoginWidget::TrySignUp(FString input_id, FString input_pw)
{
	Login::REQ_JOIN JoinPkt;

	std::string idString = TCHAR_TO_UTF8(*input_id);
	std::string pwString = TCHAR_TO_UTF8(*input_pw);
	JoinPkt.set_id(idString);
	JoinPkt.set_pw(pwString);

	SEND_PACKET(GetGameInstance<UJ1GameInstance>(), ESessionType::Login, Login::PacketType::PKT_REQ_JOIN, JoinPkt);
}

// ═════════════════════
//      로그인 관련 
// ═════════════════════
void UJ1LoginWidget::OnResultLogin(ELoginMode loginMode, bool result)
{
	FString msg;
	switch (loginMode)
	{
	case ELoginMode::Login:
		msg = result ? TEXT("로그인 성공!") : TEXT("로그인 실패..");
		if (result)
		{
			UGameplayStatics::OpenLevel(GetWorld(), FName("L_Lobby"));
			
			if (auto* GI = GetGameInstance<UJ1GameInstance>())
			{
				GI->RequestDisconnect(ESessionType::Login);
				GI->ConnectToServer(ESessionType::Game);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("GI Is Not Valid")));
			}
			
		}
		break;
	case ELoginMode::CheckId:
		msg = result ?  TEXT("사용불가한 ID입니다..") : TEXT("사용가능한 ID입니다!");
		result ? Tab_Join->SetVisibility(ESlateVisibility::Hidden) : Tab_Join->SetVisibility(ESlateVisibility::Visible);
		break;
	case ELoginMode::SignUp:
		msg = result ? TEXT("회원가입에 성공했습니다!") : TEXT("회원가입에 실패했습니다..");
		if (result)
		{
			SwitchAuthMode();
			bLoginMode = true;
		}
		break;
	
	}
	Txt_Warning->SetVisibility(ESlateVisibility::Visible);
	Txt_Warning->SetText(FText::FromString(msg));
}

void UJ1LoginWidget::SwitchAuthMode()
{
	if (bLoginMode)		// 로그인 -> 회원가입
	{
		// 회원가입 UI에 맞게 컴포넌트 visible 변경
		Tab_Login->SetVisibility(ESlateVisibility::Hidden);
		Tab_Join->SetVisibility(ESlateVisibility::Hidden);
		Tab_CheckID->SetVisibility(ESlateVisibility::Visible);
		SizeBox_CheckPW->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		// 로그인 UI에 맞게 컴포넌트 visible 변경
		Tab_Login->SetVisibility(ESlateVisibility::Visible);
		Tab_Join->SetVisibility(ESlateVisibility::Visible);
		Tab_CheckID->SetVisibility(ESlateVisibility::Hidden);
		SizeBox_CheckPW->SetVisibility(ESlateVisibility::Hidden);
	}
}
