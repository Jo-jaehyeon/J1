#pragma once

#include "Engine/DataTable.h"
#include "J1CharacterCustomizeTypes.generated.h"

// ───────────────────────
//              캐릭터 클래스 열거형
// ───────────────────────
UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
    Warrior   UMETA(DisplayName = "전사"),
    Archer    UMETA(DisplayName = "궁수"),
    Rogue     UMETA(DisplayName = "도적"),
};


// ───────────────────────
//                스킨 슬롯 열거형
// ───────────────────────
UENUM(BlueprintType)
enum class ESkinSlot : uint8
{
    Upper  UMETA(DisplayName = "상체"),
    Lower  UMETA(DisplayName = "하체"),
    Weapon UMETA(DisplayName = "무기"),
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

    /** 표시 이름 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText                          DisplayName;

    /* 설명 텍스트 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText                          Description;

    /* 클래스 스켈레탈 메시 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<USkeletalMesh>  SkeletalMesh;

    /* 클래스 애님 블루프린트 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftClassPtr<UAnimInstance>   AnimBlueprint;

    /* 상체 스킨 목록 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FSkinEntry>             UpperSkins;

    /** 하체 스킨 목록 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FSkinEntry>             LowerSkins;

    /** 무기 스킨 목록 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FSkinEntry>             WeaponSkins;

    /** 슬롯 열거형(읽기 편의용) */
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
