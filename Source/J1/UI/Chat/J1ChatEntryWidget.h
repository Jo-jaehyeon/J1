// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "J1ChatEntryWidget.generated.h"

/**
 *      Forward Declaration
 */
class UTextBlock;
class UButton;
class FOnSenderNameClicked;


UCLASS()
class J1_API UJ1ChatEntryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

public:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_Time;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_Sender;

    // 닉네임에 올려두는 투명 버튼 (클릭 감지용)
    UPROPERTY(meta = (BindWidgetOptional))
    UButton* Btn_Sender;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_Content;
};
