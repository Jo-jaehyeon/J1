// Copyright © 2026 Jerry. All rights reserved.

#include "UI/Customizing/J1CustomizingWidget.h"

void UJ1CustomizingWidget::NativeConstruct()
{
}

void UJ1CustomizingWidget::ApplyClass(ECharacterClass NewClass)
{
}

FCharacterSkinBaseData* UJ1CustomizingWidget::FindClassData(ECharacterClass InClass) const
{
	return nullptr;
}

bool UJ1CustomizingWidget::IsNicknameValid(const FString& Nickname) const
{
	return false;
}

void UJ1CustomizingWidget::OnSkinChanged(ESkinSlot SkinSlot, FSkinEntry NewSkin)
{
}

void UJ1CustomizingWidget::OnConfirmClicked()
{
}
void UJ1CustomizingWidget::OnBackClicked()
{
}