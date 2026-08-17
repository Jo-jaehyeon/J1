// Copyright © 2026 Jerry. All rights reserved.

#include "UI/Item/J1ItemDragWidget.h"
#include "Components/Image.h"

void UJ1ItemDragWidget::SetIconTexture(UTexture2D* Texture)
{
	if (Img_Icon && Texture)
	{
		Img_Icon->SetBrushFromTexture(Texture);
	}
}