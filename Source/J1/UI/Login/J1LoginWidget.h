// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "J1LoginWidget.generated.h"

/*
 *      Forward Declaration
 */
class UTextBlock;
class UEditableTextBox;
class UButton;

UCLASS()
class J1_API UJ1LoginWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    // ════════════════════════════════════
    //  위젯 바인딩 함수
    // ════════════════════════════════════
    UFUNCTION()
    void OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);

    UFUNCTION() void OnTabLogin();
    UFUNCTION() void OnTabJoin();

public:
    // ════════════════════════════════════
    //  UMG 바인딩
    // ════════════════════════════════════
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_Warning;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* Input_ID;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* Input_PW;

    UPROPERTY(meta = (BindWidget))
    UButton* Tab_Login;

    UPROPERTY(meta = (BindWidget))
    UButton* Tab_Join;
};
