// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "J1ChatTypes.h"
#include "J1ChatWidget.generated.h"

/**
 *      Forward Declaration
 */
class UScrollBox;
class UTextBlock;
class UEditableText;
class UButton;
class UJ1ChatEntryWidget;

UCLASS()
class J1_API UJ1ChatWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;


    // ════════════════════════════════════
    //  블루프린트 호출 함수
    // ════════════════════════════════════
    UFUNCTION(BlueprintCallable, Category = "Chat")
    void SetActiveChannel(EChatChannel Channel);

private:
    // ════════════════════════════════════
    //  위젯 바인딩 함수
    // ════════════════════════════════════
    UFUNCTION() void OnTabAll();
    UFUNCTION() void OnTabSystem();
    UFUNCTION() void OnTabWhisper();
    UFUNCTION() void OnTabParty();
    UFUNCTION() void OnTabGuild();

    UFUNCTION() 
    void OnInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

    // ════════════════════════════════════
    //  채팅 관련 함수
    // ════════════════════════════════════
    void AppendMessageToLog(const FText& Text);

public:
    // ════════════════════════════════════
    //  채팅 UI Setting
    // ════════════════════════════════════
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat|Settings")
    TSubclassOf<UJ1ChatEntryWidget> ChatEntryWidgetClass;

private:
    EChatChannel ActiveChannel = EChatChannel::All;

public:
    // ════════════════════════════════════
    //  UMG 바인딩
    // ════════════════════════════════════

    UPROPERTY(meta = (BindWidget))
    UScrollBox* ScrollBox_ChatLog;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_ActiveChannel;

    UPROPERTY(meta = (BindWidget))
    UEditableText* EditableText_Input;

    UPROPERTY(meta = (BindWidget))
    UButton* Tab_All;

    UPROPERTY(meta = (BindWidget))
    UButton* Tab_Party;

    UPROPERTY(meta = (BindWidget))
    UButton* Tab_Guild;

    UPROPERTY(meta = (BindWidget))
    UButton* Tab_Whisper;

    UPROPERTY(meta = (BindWidget))
    UButton* Tab_System;


    // ════════════════════════════════════
    //  채널 색상
    // ════════════════════════════════════

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat|Colors")
    FLinearColor Color_All = FLinearColor(0.63f, 0.78f, 1.00f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat|Colors")
    FLinearColor Color_Party = FLinearColor(0.25f, 0.82f, 0.38f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat|Colors")
    FLinearColor Color_Guild = FLinearColor(0.75f, 0.38f, 1.00f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat|Colors")
    FLinearColor Color_Whisper = FLinearColor(1.00f, 0.53f, 0.80f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat|Colors")
    FLinearColor Color_System = FLinearColor(0.91f, 0.63f, 0.13f, 1.0f);
};
