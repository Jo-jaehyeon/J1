// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/J1CharacterCustomizeTypes.h"
#include "J1LobbyCharacterActor.generated.h"

class USkeletalMeshComponent;
class UWidgetComponent;
class UCharacterInfoWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyCharacterClicked, int32, ClickedCharacterId);

UCLASS()
class J1_API AJ1LobbyCharacterActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AJ1LobbyCharacterActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Lobby")	void SetFilledSlot(const FLobbySlotInfo& InInfo);
	UFUNCTION(BlueprintCallable, Category = "Lobby")	void SetEmptySlot(int32 InSlotIndex);
	UFUNCTION(BlueprintPure, Category = "Lobby")		bool IsEmptySlot() const { return bIsEmptySlot; }
	UFUNCTION(BlueprintCallable, Category = "Lobby")	void SetSelected(bool bInSelected);

	UFUNCTION(BlueprintPure, Category = "Lobby")	
	int32 GetCharacterSlotIdx() const { return CachedInfo.SlotIndex; }
	UFUNCTION(BlueprintPure, Category = "Lobby")
	const FLobbySlotInfo& GetCharacterInfo() const { return CachedInfo; }

protected:
	const FCharacterSkinBaseData* FindClassRow(ECharacterClass InClass) const;

	void StartAsyncLoad(const FCharacterSkinBaseData& ClassRow, const FLobbySlotInfo& InInfo);
	void HandleAssetsLoaded(uint32 RequestId, ECharacterClass LoadedClassType);

	void ApplySkinMaterial(const FCharacterSkinBaseData& ClassRow, ESkinSlot Slot, int32 SkinIndex);

	UFUNCTION()
	void HandleInfoWidgetClicked();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")	
	TObjectPtr<USkeletalMeshComponent> BodyMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")	
	TObjectPtr<UWidgetComponent> InfoWidgetComp;

	// ═════════════════════
	//		 DELEGATE
	// ═════════════════════
	UPROPERTY(BlueprintAssignable, Category = "Lobby")						
	FOnLobbyCharacterClicked OnCharacterClicked;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Lobby|DataTable")	
	TObjectPtr<UDataTable> CharacterBaseDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby|Fallback")
	TObjectPtr<UMaterialInterface> FallbackMaterial;

	UPROPERTY()		FLobbySlotInfo CachedInfo;
	UPROPERTY()		bool bIsSelected = false;
	UPROPERTY()		bool bIsEmptySlot = false;

	// 현재 진행 중인 비동기 로드 요청 핸들.
	TSharedPtr<struct FStreamableHandle> ActiveLoadHandle;

	// InitFromServerInfo가 호출될 때마다 증가하는 세대 값.
	// 로드가 끝나기 전에 다른 데이터로 다시 초기화되면, 낡은(stale) 콜백이 화면에 반영되지 않도록 막는 용도.
	uint32 LoadRequestGeneration = 0;
};
