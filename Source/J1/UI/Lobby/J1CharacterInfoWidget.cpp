// Copyright © 2026 Jerry. All rights reserved.

#include "UI/Lobby/J1CharacterInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

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

void UJ1CharacterInfoWidget::SetData(const FGuid& InCharacterId, const FString& InName, ECharacterClass InClass, int32 InLevel)
{
	CharacterUniqueID = InCharacterId;

	if (Txt_Class)	Txt_Class->SetText(GetClassDisplayName(InClass));
	if (Txt_Name)	Txt_Name->SetText(FText::FromString(InName));
	if (Txt_Level)	Txt_Level->SetText(FText::Format(NSLOCTEXT("Lobby", "LevelFormat", "Lv.{0}"), FText::AsNumber(InLevel)));
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
