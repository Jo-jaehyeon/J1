// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "J1LobbyWidget.generated.h"

/*
 *      Forward Declaration
 */
class UOverlay;
class UTextBlock;
class UButton;
class UJ1LobbyDisplayManager;

UCLASS()
class J1_API UJ1LobbyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetDisplayManager(UJ1LobbyDisplayManager* InManager);

protected:
	UFUNCTION()	void HandleStartGameClicked();
	UFUNCTION()	void HandleDeleteCharacterClicked();
	UFUNCTION()	void HandleCreateCharacterClicked();
	UFUNCTION()	void HandleSelectionChanged(bool bHasValidSelection);

private:
	void OnNotice(FString text);

protected:
	// ════════════════════════════════════
	//              UMG 바인딩
	// ════════════════════════════════════
	UPROPERTY(meta = (BindWidget))  TObjectPtr<UOverlay>	 Overlay_Notice;
	UPROPERTY(meta = (BindWidget))  TObjectPtr<UTextBlock>	 Txt_Notice;
	UPROPERTY(meta = (BindWidget))	TObjectPtr<UButton>		 Btn_StartGame;
	UPROPERTY(meta = (BindWidget))	TObjectPtr<UButton>		 Btn_DeleteCharacter;
	UPROPERTY(meta = (BindWidget))	TObjectPtr<UButton>		 Btn_CreateCharacter;

	UPROPERTY()						TObjectPtr<UJ1LobbyDisplayManager> DisplayManager;

	UPROPERTY()
	FTimerHandle PopupVisibilityTimerHandle;
};
