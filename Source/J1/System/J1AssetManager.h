// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "J1AssetManager.generated.h"

class UJ1ItemData;
class UJ1UIData;

UCLASS(Config = Game)
class J1_API UJ1AssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UJ1AssetManager& Get();

    virtual void StartInitialLoading() override;

    const UJ1ItemData& GetItemData();
    const UJ1UIData& GetUIData();

    template <typename GameDataClass>
    const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
    {
        if (TObjectPtr<UPrimaryDataAsset> const* pResult = GameDataMap.Find(GameDataClass::StaticClass()))
        {
            return *CastChecked<GameDataClass>(*pResult);
        }
        return *CastChecked<const GameDataClass>(
            LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName())
        );
    }

private:
    UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType);

public:
    UPROPERTY(Config)   TSoftObjectPtr<UJ1ItemData> ItemDataPath;
    UPROPERTY(Config)   TSoftObjectPtr<UJ1UIData>   UIDataPath;

private:
    UPROPERTY(Transient)
    TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;
};
