// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/J1CharacterCustomizeTypes.h"
#include "J1LobbyDisplayManager.generated.h"

class AJ1LobbyCharacterActor;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbySelectionChanged, bool, bHasValidSelection);

UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class J1_API UJ1LobbyDisplayManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UJ1LobbyDisplayManager();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Lobby") void SetCharacterList(const TArray<FLobbySlotInfo>& InList);
	UFUNCTION(BlueprintCallable, Category = "Lobby") void SetMaxSlotCount(int32 InMaxSlotCount);

	UFUNCTION(BlueprintCallable, Category = "Lobby") void OnClickStartGame();
	UFUNCTION(BlueprintCallable, Category = "Lobby") void OnClickDeleteCharacter();

	UFUNCTION(BlueprintPure, Category = "Lobby") 
	bool HasValidSelection() const { return SelectedSlotIdx != INDEX_NONE; }

protected:
	void					EnsureActorPool();
	void					RefreshSlots();
	void					AddCharacterLocally(FLobbySlotInfo& Info);
	void					RemoveCharacterLocally(const int32& characterid);
	void					HandleChanageCharactetList(bool Isadd, FLobbySlotInfo& Info);
	FLobbySlotInfo*			FindCharacterInfo(int32 CharacterId);
	FLobbySlotInfo*			FindCharacterInfoBySlotIndex(int32 InSlotIndex);

	UFUNCTION()
	void HandleCharacterClicked(int32 ClickedCharacterId);

public:
	// ═════════════════════
	//		 DELEGATE
	// ═════════════════════
	UPROPERTY(BlueprintAssignable, Category = "Lobby")	
	FOnLobbySelectionChanged OnSelectionChanged;

protected:
	UPROPERTY(EditAnywhere, Category = "Lobby|Layout")
	TArray<FTransform> SlotTransforms;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby|Layout")
	TSubclassOf<AJ1LobbyCharacterActor> CharacterActorClass;

	UPROPERTY(EditAnywhere, Category = "Lobby|Layout")
	int32 TotalCharacterSlotCount = 5;

	UPROPERTY()	TArray<FLobbySlotInfo> CharacterList;
	UPROPERTY()	TArray<TObjectPtr<AJ1LobbyCharacterActor>> PooledActors;
	UPROPERTY()	int32 SelectedSlotIdx = INDEX_NONE;
};
