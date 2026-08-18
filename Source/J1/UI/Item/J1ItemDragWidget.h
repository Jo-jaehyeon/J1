// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "J1ItemDragWidget.generated.h"

class UImage;

// 드래그 중 마우스를 따라다니는 아이콘 전용 위젯.
UCLASS()
class J1_API UJ1ItemDragWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetIconTexture(UTexture2D* Texture);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UImage* Img_Icon;
};
