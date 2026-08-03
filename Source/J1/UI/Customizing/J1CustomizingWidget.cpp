// Copyright © 2026 Jerry. All rights reserved.

#include "UI/Customizing/J1CustomizingWidget.h"
#include "J1SkinSelectWidget.h"
#include "Character/PC/J1CustomizePreviewActor.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Internationalization/Regex.h"

void UJ1CustomizingWidget::NativeConstruct()
{
	// 클래스 선택 버튼 바인딩
	if (Btn_Warrior)	Btn_Warrior->OnClicked.AddDynamic(this, &UJ1CustomizingWidget::OnWarriorClicked);
	if (Btn_Archer)		Btn_Archer->OnClicked.AddDynamic(this, &UJ1CustomizingWidget::OnArcherClicked);
	if (Btn_Assassin)	Btn_Assassin->OnClicked.AddDynamic(this, &UJ1CustomizingWidget::OnAssassinClicked);


	// 스킨 셀렉트 위젯 바인딩
	if (WBP_SkinSelect_Upper)	WBP_SkinSelect_Upper->OnSkinChanged.AddDynamic(this, &UJ1CustomizingWidget::OnSkinChanged);
	if (WBP_SkinSelect_Lower)	WBP_SkinSelect_Lower->OnSkinChanged.AddDynamic(this, &UJ1CustomizingWidget::OnSkinChanged);
	if (WBP_SkinSelect_Weapon)	WBP_SkinSelect_Weapon->OnSkinChanged.AddDynamic(this, &UJ1CustomizingWidget::OnSkinChanged);

	// 확정/캔슬 버튼 바인딩
	if (Btn_Confirm)	Btn_Confirm->OnClicked.AddDynamic(this, &UJ1CustomizingWidget::OnConfirmClicked);
	if (Btn_Back)		Btn_Back->OnClicked.AddDynamic(this, &UJ1CustomizingWidget::OnBackClicked);

	// 씬에서 프리뷰 액터 자동 탐색
	if (!PreviewActor)
	{
		AActor* Found = UGameplayStatics::GetActorOfClass(GetWorld(), AJ1CustomizePreviewActor::StaticClass());
		PreviewActor = Cast<AJ1CustomizePreviewActor>(Found);
	}

	// 기본 전사 적용
	ApplyClass(ECharacterClass::Warrior);
}

void UJ1CustomizingWidget::ApplyClass(ECharacterClass NewClass)
{
	CurrentClass = NewClass;

	FCharacterSkinBaseData* Data = FindClassData(NewClass);
	if (!Data) return;


	// 프리뷰 액터 -> 스켈레탈 메시 + 애님 교체
	if (PreviewActor)		PreviewActor->ApplyClass(*Data);
	
	// 새 스킨 목록 주입
	if (WBP_SkinSelect_Upper)  WBP_SkinSelect_Upper->SetSkins(Data->UpperSkins);
	if (WBP_SkinSelect_Lower)  WBP_SkinSelect_Lower->SetSkins(Data->LowerSkins);
	if (WBP_SkinSelect_Weapon) WBP_SkinSelect_Weapon->SetSkins(Data->WeaponSkins);


	// 기본 스킨(인덱스 0)을 즉시 프리뷰 액터에도 반영
	auto ApplyDefault = [&](UJ1SkinSelectWidget* Selecter, ESkinSlot NewSlot)
		{
			if (Selecter && PreviewActor)
				PreviewActor->ApplySkin(NewSlot, Selecter->GetCurrentSkin());
		};
	ApplyDefault(WBP_SkinSelect_Upper, ESkinSlot::Upper);
	ApplyDefault(WBP_SkinSelect_Lower, ESkinSlot::Lower);
	ApplyDefault(WBP_SkinSelect_Weapon, ESkinSlot::Weapon);
}

FCharacterSkinBaseData* UJ1CustomizingWidget::FindClassData(ECharacterClass InClass) const
{
	if (!CharacterDataTable) return nullptr;

	const UEnum* EnumPtr = StaticEnum<ECharacterClass>();
	if (!EnumPtr) return nullptr;

	FString RowName = EnumPtr->GetNameStringByValue(static_cast<int64>(InClass));

	return CharacterDataTable->FindRow<FCharacterSkinBaseData>(FName(*RowName), TEXT("FindClassData"));
}

// 닉네임 유효성 검사(2~12자, 한글/영어/숫자)
bool UJ1CustomizingWidget::IsNicknameValid(const FString& Nickname) const
{
	const int32 Len = Nickname.Len();
	if (Len < 2 || Len > 12) return false;

	const FRegexPattern Pattern(TEXT("^[가-힣a-zA-Z0-9]+$"));
	FRegexMatcher Matcher(Pattern, Nickname);
	return Matcher.FindNext();
}

void UJ1CustomizingWidget::OnSkinChanged(ESkinSlot SkinSlot, FSkinEntry NewSkin)
{
	if (PreviewActor)
		PreviewActor->ApplySkin(SkinSlot, NewSkin);
}

void UJ1CustomizingWidget::OnConfirmClicked()
{
	FString Nickname = EditableText_Nickname ? EditableText_Nickname->GetText().ToString() : TEXT("");

	// 가능한 닉네임인지 체크
	if (!IsNicknameValid(Nickname))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Customize] 닉네임 유효성 검사 실패: %s"), *Nickname);
		// TODO: 별도 UI 오픈
		return;
	}

	// 중복되지 않은 닉네임인지 체크
	// TODO : 서버파트


	// 결과 서버 전달
	//FCharacterCustomizeResult Result;
	//Result.Nickname = Nickname;
	//Result.SelectedClass = CurrentClass;
	//Result.UpperSkinIndex = WBP_SkinSelect_Upper ? WBP_SkinSelect_Upper->GetCurrentIndex() : 0;
	//Result.LowerSkinIndex = WBP_SkinSelect_Lower ? WBP_SkinSelect_Lower->GetCurrentIndex() : 0;
	//Result.WeaponSkinIndex = WBP_SkinSelect_Weapon ? WBP_SkinSelect_Weapon->GetCurrentIndex() : 0;

	// TODO 전달
	
	// UGameplayStatics::OpenLevel(GetWorld(), FName("GameMap"));

}

void UJ1CustomizingWidget::OnBackClicked()
{
	//UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenuMap"));
}