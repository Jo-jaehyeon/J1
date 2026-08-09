// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/J1CharacterCustomizeTypes.h"
#include "J1CharacterInfoWidget.generated.h"

/*
 *      Forward Declaration
 */
class UTextBlock;
class UButton;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterInfoClicked);

UCLASS()
class J1_API UJ1CharacterInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetData(const FGuid& InCharacterId, const FString& InName, ECharacterClass InClass, int32 InLevel);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetSelectedStyle(bool bSelected);

	UFUNCTION(BlueprintPure, Category = "Lobby")
	FGuid GetCharacterUniqueID() const { return CharacterUniqueID; }

protected:
	UFUNCTION()
	void HandleButtonClicked();

	FText GetClassDisplayName(ECharacterClass InClass) const;

public:
	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnCharacterInfoClicked OnWidgetClicked;

protected:
	// ════════════════════════════════════
	//              UMG 바인딩
	// ════════════════════════════════════
	UPROPERTY(meta = (BindWidget))	TObjectPtr<UButton>		Btn_ClickArea;
	UPROPERTY(meta = (BindWidget))	TObjectPtr<UTextBlock>	Txt_Class;
	UPROPERTY(meta = (BindWidget))	TObjectPtr<UTextBlock>	Txt_Name;
	UPROPERTY(meta = (BindWidget))	TObjectPtr<UTextBlock>	Txt_Level;
	UPROPERTY(meta = (BindWidget))	TObjectPtr<UImage>		Img_SelectedCheck;


	FGuid CharacterUniqueID;
};
