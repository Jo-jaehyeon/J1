// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Types/J1CharacterCustomizeTypes.h"
#include "J1CustomizingWidget.generated.h"


/*
 *      Forward Declaration
 */
class UButton;
class UEditableTextBox;
class UTextBlock;
class UOverlay;
class UJ1SkinSelectWidget;
class UDataTable;
class AJ1CustomizePreviewActor;

/**
 * 
 */
UCLASS()
class J1_API UJ1CustomizingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    // 클래스 적용: 메시 교체 + 피커에 TArray 주입
    void ApplyClass(ECharacterClass NewClass);

    // 통합 DataTable에서 행 조회
    FCharacterSkinBaseData* FindClassData(ECharacterClass InClass) const;

    // 닉네임 유효성 검사 (2~12자, 한글/영문/숫자)
    bool IsNicknameValid(const FString& Nickname) const;

private:
    // 클래스 버튼 콜백
    UFUNCTION() void OnWarriorClicked() { ApplyClass(ECharacterClass::Warrior); };
    UFUNCTION() void OnArcherClicked()  { ApplyClass(ECharacterClass::Archer); };
    UFUNCTION() void OnAssassinClicked()   { ApplyClass(ECharacterClass::Assassin); };

    // 스킨 피커 델리게이트 콜백
    UFUNCTION() void OnSkinChanged(ESkinSlot SkinSlot, FSkinEntry NewSkin);

    // 최종 버튼 콜백
    UFUNCTION() void OnConfirmClicked();
    void HandleCheckNickName(bool check);

    UFUNCTION() void OnBackClicked();
    UFUNCTION() void OnCreateClicked();
    UFUNCTION() void OnCancleClicked();

protected:
    // ════════════════════════════════════
    //				UMG 바인딩
    // ════════════════════════════════════
    UPROPERTY(meta = (BindWidget)) UButton* Btn_Warrior;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_Archer;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_Assassin;

    UPROPERTY(meta = (BindWidget)) UJ1SkinSelectWidget* WBP_SkinSelect_Upper;
    UPROPERTY(meta = (BindWidget)) UJ1SkinSelectWidget* WBP_SkinSelect_Lower;
    UPROPERTY(meta = (BindWidget)) UJ1SkinSelectWidget* WBP_SkinSelect_Weapon;

    UPROPERTY(meta = (BindWidget)) UEditableTextBox*    EditableText_Nickname;
    UPROPERTY(meta = (BindWidget)) UButton*             Btn_Confirm;
    UPROPERTY(meta = (BindWidget)) UButton*             Btn_Back;
    UPROPERTY(meta = (BindWidget)) UTextBlock*          Txt_Notice;

    UPROPERTY(meta = (BindWidget)) UOverlay*            Widget_PopUp;
    UPROPERTY(meta = (BindWidget)) UTextBlock*          Txt_Check;
    UPROPERTY(meta = (BindWidget)) UButton*             Btn_Create;
    UPROPERTY(meta = (BindWidget)) UButton*             Btn_Cancle;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customize")
    UDataTable* CharacterDataTable;

    /** 씬에 배치된 프리뷰 액터 */
    UPROPERTY(BlueprintReadWrite, Category = "Customize")
    AJ1CustomizePreviewActor* PreviewActor;

private:
    ECharacterClass CurrentClass = ECharacterClass::Warrior;
};
