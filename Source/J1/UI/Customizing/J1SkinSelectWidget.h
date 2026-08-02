// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/J1CharacterCustomizeTypes.h"
#include "J1SkinSelectWidget.generated.h"


/*
 *      Forward Declaration
 */
class UButton;
class UTextBlock;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkinChanged, ESkinSlot, Slot, FSkinEntry, Entry);

/**
 * 
 */
UCLASS()
class J1_API UJ1SkinSelectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Skin Picker")
	void SetSkins(const TArray<FSkinEntry>& NewSkins);

	/** 현재 선택된 FSkinEntry 반환 */
	UFUNCTION(BlueprintPure, Category = "Skin Picker")
	FSkinEntry GetCurrentSkin() const;

	/** 현재 인덱스 반환 (결과 저장용) */
	UFUNCTION(BlueprintPure, Category = "Skin Picker")
	int32 GetCurrentIndex() const { return CurrentIndex; }

private:
	void UpdateDisplay();

	UFUNCTION() void OnPrevClicked();
	UFUNCTION() void OnNextClicked();


protected:
	// ════════════════════════════════════
	//  UMG 바인딩
	// ════════════════════════════════════
	UPROPERTY(meta = (BindWidget)) UButton*		Btn_Prev;
	UPROPERTY(meta = (BindWidget)) UButton*		Btn_Next;
	UPROPERTY(meta = (BindWidget)) UImage*		Img_Thumbnail;
	UPROPERTY(meta = (BindWidget)) UTextBlock*	Txt_Name;
	UPROPERTY(meta = (BindWidget)) UTextBlock*	Txt_Index;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SkinSelect")
	ESkinSlot TargetSlot = ESkinSlot::Upper;

	UPROPERTY(BlueprintAssignable, Category="SkinSelect")
	FOnSkinChanged OnSkinChanged;

private:
	TArray<FSkinEntry> CurrentSkins;
	int32 CurrentIndex = 0;
};
