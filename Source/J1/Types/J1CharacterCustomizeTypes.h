#pragma once

#include "Engine/DataTable.h"
#include "J1CharacterCustomizeTypes.generated.h"

// ───────────────────────
//              캐릭터 클래스 열거형
// ───────────────────────
UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
    Warrior         UMETA(DisplayName = "Warrior"),
    Archer          UMETA(DisplayName = "Archer"),
    Assassin        UMETA(DisplayName = "Assassin"),
};


// ───────────────────────
//                스킨 슬롯 열거형
// ───────────────────────
UENUM(BlueprintType)
enum class ESkinSlot : uint8
{
    Upper           UMETA(DisplayName = "상체"),
    Lower           UMETA(DisplayName = "하체"),
    Weapon          UMETA(DisplayName = "무기"),
};


// ───────────────────────
//                스킨 항목 하나
// ───────────────────────
USTRUCT(BlueprintType)
struct FSkinEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText                                DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UMaterialInterface>   Material;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UTexture2D>           Thumbnail;
};


// ───────────────────────
//           DataTable 행: 클래스 + 스킨
// ───────────────────────
USTRUCT(BlueprintType)
struct FCharacterSkinBaseData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText                          DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText                          Description;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<USkeletalMesh>  SkeletalMesh;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftClassPtr<UAnimInstance>   AnimBlueprint;
    
    // Skin Material
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FSkinEntry>             UpperSkins;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FSkinEntry>             LowerSkins;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FSkinEntry>             WeaponSkins;


    // ────────────────────────
    //    Material별로 적용해야 하는 인덱스 목록.
    // ────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<int32>                  UpperMaterialSlotIndices;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<int32>                  LowerMaterialSlotIndices;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<int32>                  WeaponMaterialSlotIndices;

    // 슬롯 열거형(읽기 편의용) 
    const TArray<FSkinEntry>* GetSkinsForSlot(ESkinSlot Slot) const
    {
        switch (Slot)
        {
        case ESkinSlot::Upper:  return &UpperSkins;
        case ESkinSlot::Lower:  return &LowerSkins;
        case ESkinSlot::Weapon: return &WeaponSkins;
        default:                return nullptr;
        }
    }
    const TArray<int32>* GetMaterialSlotIndicesForSlot(ESkinSlot Slot) const
    {
        switch (Slot)
        {
        case ESkinSlot::Upper:  return &UpperMaterialSlotIndices;
        case ESkinSlot::Lower:  return &LowerMaterialSlotIndices;
        case ESkinSlot::Weapon: return &WeaponMaterialSlotIndices;
        default:                return nullptr;
        }
    }
};

// ───────────────────────
//             커스터마이징 최종 결과
// ───────────────────────
USTRUCT(BlueprintType)
struct FCharacterCustomizeResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite) FString         Nickname;
    UPROPERTY(BlueprintReadWrite) ECharacterClass SelectedClass = ECharacterClass::Warrior;
    UPROPERTY(BlueprintReadWrite) int32           UpperSkinIndex = 0;
    UPROPERTY(BlueprintReadWrite) int32           LowerSkinIndex = 0;
    UPROPERTY(BlueprintReadWrite) int32           WeaponSkinIndex = 0;
 
};

// ───────────────────────
//           로비 캐릭터를 구성하는 데이터 
// ───────────────────────
USTRUCT(BlueprintType)
struct FLobbySlotInfo
{
    GENERATED_BODY()

    // 서버에서 캐릭터를 식별하기 위한 고유 ID.
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Lobby")  int32 CharacterUniqueID = INDEX_NONE;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Lobby")  int32 SlotIndex = 0;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Lobby")  ECharacterClass ClassType = ECharacterClass::Warrior;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Lobby")  FString CharacterName;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Lobby")  int32 Level = 1;

    // DT_SkinMaterial의 Material index
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Lobby")  int32 UpperBodySkinRowID;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Lobby")  int32 LowerBodySkinRowID;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Lobby")  int32 WeaponSkinRowID;
};
