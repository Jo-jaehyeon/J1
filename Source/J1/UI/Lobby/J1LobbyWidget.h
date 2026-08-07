// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "J1LobbyWidget.generated.h"

/*
 *      Forward Declaration
 */
class USizeBox;
class UTextBlock;

UCLASS()
class J1_API UJ1LobbyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void OnNotice(FString text);

public:
	// ════════════════════════════════════
	//              UMG 바인딩
	// ════════════════════════════════════
	UPROPERTY(meta = (BindWidget))  USizeBox*	SizeBox_Notice;
	UPROPERTY(meta = (BindWidget))  UTextBlock* Txt_Notice;
	
};
