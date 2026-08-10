// Copyright © 2026 Jerry. All rights reserved.

#include "UI/Lobby/J1CharacterInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "J1GameInstance.h"

void UJ1CharacterInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_ClickArea)
	{
		Btn_ClickArea->OnClicked.RemoveDynamic(this, &UJ1CharacterInfoWidget::HandleButtonClicked);
		Btn_ClickArea->OnClicked.AddDynamic(this, &UJ1CharacterInfoWidget::HandleButtonClicked);
	}

	SetSelectedStyle(false);
}

void UJ1CharacterInfoWidget::SetFilledData(int32 InCharacterId, const FString& InName, ECharacterClass InClass, int32 InLevel)
{
	CharacterUniqueID = InCharacterId;
	bIsEmptyState = false;
	SlotIndex = INDEX_NONE;

	if (Txt_Class)
	{
		Txt_Class->SetText(GetClassDisplayName(InClass));
		Txt_Class->SetVisibility(ESlateVisibility::Visible);
	}
	if (Txt_Name)
	{
		Txt_Name->SetText(FText::FromString(InName));
		Txt_Name->SetVisibility(ESlateVisibility::Visible);
	}
	if (Txt_Level)
	{
		Txt_Level->SetText(FText::Format(NSLOCTEXT("Lobby", "LevelFormat", "Lv.{0}"), FText::AsNumber(InLevel)));
		Txt_Level->SetVisibility(ESlateVisibility::Visible);
	}
}

void UJ1CharacterInfoWidget::SetEmptyData(int32 InSlotIndex)
{
	CharacterUniqueID = INDEX_NONE;
	bIsEmptyState = true;
	SlotIndex = InSlotIndex;

	if (Txt_Name)
	{
		Txt_Name->SetText(NSLOCTEXT("Lobby", "EmptySlotLabel", "캐릭터 생성 하기"));
		Txt_Name->SetVisibility(ESlateVisibility::Visible);
	}
	if (Txt_Class)	Txt_Class->SetVisibility(ESlateVisibility::Collapsed);
	if (Txt_Level)	Txt_Level->SetVisibility(ESlateVisibility::Collapsed);
	

	SetSelectedStyle(false);
}

void UJ1CharacterInfoWidget::SetSelectedStyle(bool bSelected)
{
	if (Img_SelectedCheck)
	{
		Img_SelectedCheck->SetVisibility(bSelected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UJ1CharacterInfoWidget::HandleButtonClicked()
{
	if (bIsEmptyState)
	{
		UGameplayStatics::OpenLevel(this, "L_Customize");
		return;
	}
	
	OnWidgetClicked.Broadcast();
}

FText UJ1CharacterInfoWidget::GetClassDisplayName(ECharacterClass InClass) const
{
	// UENUM DisplayName을 그대로 활용
	const UEnum* EnumPtr = StaticEnum<ECharacterClass>();
	if (!EnumPtr)
	{
		return FText::GetEmpty();
	}
	return EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(InClass));
}
