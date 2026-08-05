// Copyright © 2026 Jerry. All rights reserved.


#include "UI/Customizing/J1SkinSelectWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

void UJ1SkinSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Prev) Btn_Prev->OnClicked.AddDynamic(this, &UJ1SkinSelectWidget::OnPrevClicked);
	if (Btn_Next) Btn_Next->OnClicked.AddDynamic(this, &UJ1SkinSelectWidget::OnNextClicked);
}

void UJ1SkinSelectWidget::SetSkins(const TArray<FSkinEntry>& NewSkins)
{
	CurrentSkins = NewSkins;
	CurrentIndex = 0;
	UpdateDisplay();
}

FSkinEntry UJ1SkinSelectWidget::GetCurrentSkin() const
{
	if (CurrentSkins.IsValidIndex(CurrentIndex))
		return CurrentSkins[CurrentIndex];
	return FSkinEntry{};
}

void UJ1SkinSelectWidget::UpdateDisplay()
{
	if (CurrentSkins.IsEmpty())	return;

	const FSkinEntry& Current = CurrentSkins[CurrentIndex];

	// 스킨 썸네일 비동기 로드
	if (Img_Thumbnail)
	{
		if (UTexture2D* Tex = Current.Thumbnail.Get())
		{
			Img_Thumbnail->SetBrushFromTexture(Tex);
		}
		else if (!Current.Thumbnail.IsNull())
		{
			FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
			Streamable.RequestAsyncLoad(
				Current.Thumbnail.ToSoftObjectPath(),
				[this, Index = CurrentIndex]()
				{
					if (Img_Thumbnail && CurrentSkins.IsValidIndex(Index))
						Img_Thumbnail->SetBrushFromTexture(CurrentSkins[Index].Thumbnail.Get());
				}
			);
		}
	}

	if (Txt_Name)	Txt_Name->SetText(Current.DisplayName);
	if (Txt_Index)
	{
		Txt_Index->SetText(FText::Format(
			NSLOCTEXT("SkinSelect", "Idx", "{0} / {1}"),
			FText::AsNumber(CurrentIndex + 1),
			FText::AsNumber(CurrentSkins.Num())
		));
	}
}

void UJ1SkinSelectWidget::OnPrevClicked()
{
	if (CurrentSkins.IsEmpty()) return;
	CurrentIndex = (CurrentIndex - 1 + CurrentSkins.Num()) % CurrentSkins.Num();
	UpdateDisplay();
	OnSkinChanged.Broadcast(TargetSlot, CurrentSkins[CurrentIndex]);
}

void UJ1SkinSelectWidget::OnNextClicked()
{
	if (CurrentSkins.IsEmpty()) return;
	CurrentIndex = (CurrentIndex + 1) % CurrentSkins.Num();
	UpdateDisplay();
	OnSkinChanged.Broadcast(TargetSlot, CurrentSkins[CurrentIndex]);
}
