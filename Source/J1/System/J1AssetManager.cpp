// Copyright © 2026 Jerry. All rights reserved.

#include "System/J1AssetManager.h"
#include "Data/J1ItemData.h"
#include "Data/J1UIData.h"
#include "Types/J1LogChannels.h"

UJ1AssetManager& UJ1AssetManager::Get()
{
    check(GEngine);
    return *CastChecked<UJ1AssetManager>(GEngine->AssetManager);
}

void UJ1AssetManager::StartInitialLoading()
{
    Super::StartInitialLoading();
    // 시작 시 미리 캐싱
    GetItemData(); 
    GetUIData();
}

const UJ1ItemData& UJ1AssetManager::GetItemData()
{
    return GetOrLoadTypedGameData<UJ1ItemData>(ItemDataPath);
}

const UJ1UIData& UJ1AssetManager::GetUIData()
{
    return GetOrLoadTypedGameData<UJ1UIData>(UIDataPath);
}

UPrimaryDataAsset* UJ1AssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
    UPrimaryDataAsset* Asset = nullptr;

    // 1. 소프트 경로가 비어있지 않으면 동기 로드 실행
    if (!DataClassPath.IsNull())
    {
        Asset = DataClassPath.LoadSynchronous();
    }

    if (Asset)
    {
        // 2. 로드 성공하면 캐시 맵에 저장
        GameDataMap.Add(DataClass, Asset);
    }
    else
    {
        // 3. 실패하면 에러 로그 (경로 미지정 등)
        UE_LOG(SystemLog, Error, TEXT("Failed to load PrimaryDataAsset for %s"), *PrimaryAssetType.ToString());
    }

    return Asset;
}
