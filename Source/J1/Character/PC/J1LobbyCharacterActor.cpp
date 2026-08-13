// Copyright © 2026 Jerry. All rights reserved.

#include "Character/PC/J1LobbyCharacterActor.h"
#include "UI/Lobby/J1CharacterInfoWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "UI/Lobby/J1CharacterInfoWidget.h"

// Sets default values
AJ1LobbyCharacterActor::AJ1LobbyCharacterActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	RootComponent = BodyMesh;

	InfoWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfoWidgetComp"));
	InfoWidgetComp->SetupAttachment(BodyMesh);
	InfoWidgetComp->SetWidgetSpace(EWidgetSpace::Screen); // 항상 카메라를 향하고 화면상 크기 일정
	InfoWidgetComp->SetRelativeLocation(FVector(0.f, 0.f, -10.f)); // 발밑 하단 오프셋(레벨/스케일에 맞게 조정)
	InfoWidgetComp->SetDrawSize(FVector2D(220.f, 80.f));
	InfoWidgetComp->SetVisibility(true);
}

// Called when the game starts or when spawned
void AJ1LobbyCharacterActor::BeginPlay()
{
	Super::BeginPlay();

	// InfoWidgetComp의 위젯 인스턴스는 캐릭터 있음/없음 상태와 무관하게 항상 동일하게 유지되므로
	// 클릭 델리게이트는 BeginPlay에서 한 번만 바인딩하면 된다.
	if (UJ1CharacterInfoWidget* InfoWidget = Cast<UJ1CharacterInfoWidget>(InfoWidgetComp->GetUserWidgetObject()))
	{
		InfoWidget->OnWidgetClicked.RemoveDynamic(this, &AJ1LobbyCharacterActor::HandleInfoWidgetClicked);
		InfoWidget->OnWidgetClicked.AddDynamic(this, &AJ1LobbyCharacterActor::HandleInfoWidgetClicked);
	}
}

void AJ1LobbyCharacterActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 파괴/레벨 전환 시 진행 중이던 비동기 로드 요청을 취소해 이후 콜백이 실행되지 않도록 한다.
	if (ActiveLoadHandle.IsValid())
	{
		ActiveLoadHandle->CancelHandle();
		ActiveLoadHandle.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

void AJ1LobbyCharacterActor::SetFilledSlot(const FLobbySlotInfo& InInfo)
{
	CachedInfo = InInfo;
	bIsSelected = false;
	bIsEmptySlot = false;

	BodyMesh->SetVisibility(true, false);

	// 이전에 진행 중이던 로드 요청이 있다면 취소 (풀링된 액터가 다른 캐릭터 데이터로 재사용되는 경우)
	if (ActiveLoadHandle.IsValid())
	{
		ActiveLoadHandle->CancelHandle();
		ActiveLoadHandle.Reset();
	}

	const FCharacterSkinBaseData* ClassRow = FindClassRow(InInfo.ClassType);
	if (!ClassRow)
	{
		return;
	}

	StartAsyncLoad(*ClassRow, InInfo);
}

void AJ1LobbyCharacterActor::SetEmptySlot(int32 InSlotIndex)
{
	CachedInfo = FLobbySlotInfo();
	CachedInfo.SlotIndex = InSlotIndex;
	bIsSelected = false;
	bIsEmptySlot = true;

	BodyMesh->SetVisibility(false, false);

	// 진행 중이던 캐릭터 애셋 로드가 있다면 취소 (빈 슬롯으로 전환되므로 더 이상 필요 없음)
	if (ActiveLoadHandle.IsValid())
	{
		ActiveLoadHandle->CancelHandle();
		ActiveLoadHandle.Reset();
	}

	if (UJ1CharacterInfoWidget* InfoWidget = Cast<UJ1CharacterInfoWidget>(InfoWidgetComp->GetUserWidgetObject()))
	{
		InfoWidget->SetEmptyData(InSlotIndex);
	}
}

void AJ1LobbyCharacterActor::SetSelected(bool bInSelected)
{
	bIsSelected = bInSelected;

	// TODO: 아웃라인 포스트프로세스 머티리얼 파라미터 또는 CustomDepth 등을 이용한 하이라이트 처리
	BodyMesh->SetRenderCustomDepth(bIsSelected);

	if (UJ1CharacterInfoWidget* InfoWidget = Cast<UJ1CharacterInfoWidget>(InfoWidgetComp->GetUserWidgetObject()))
	{
		InfoWidget->SetSelectedStyle(bIsSelected);
	}
}

const FCharacterSkinBaseData* AJ1LobbyCharacterActor::FindClassRow(ECharacterClass InClass) const
{
	if (!CharacterBaseDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LobbyCharacterActor] CharacterBaseDataTable이 설정되지 않았습니다."));
		return nullptr;
	}
	
	// UEnum::GetValueAsName은 "ECharacterClass::Warrior" 형태를 반환, 접두사 제거.
	const FName RowName = UEnum::GetValueAsName(InClass);
	FString RowNameStr = RowName.ToString();
	int32 ScopeIdx = INDEX_NONE;
	if (RowNameStr.FindLastChar(TEXT(':'), ScopeIdx))
	{
		RowNameStr = RowNameStr.RightChop(ScopeIdx + 1);
	}

	const FCharacterSkinBaseData* Row = CharacterBaseDataTable->FindRow<FCharacterSkinBaseData>(FName(*RowNameStr), TEXT("ClassLookup"));
	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LobbyCharacterActor] 클래스 룩업 실패: %s"), *RowNameStr);
	}

	return Row;
}

void AJ1LobbyCharacterActor::StartAsyncLoad(const FCharacterSkinBaseData& ClassRow, const FLobbySlotInfo& InInfo)
{
	// 이번 요청에 새로운 세대 번호를 부여. HandleAssetsLoaded에서 이 번호로 "낡은 콜백"인지 판별한다.
	const uint32 ThisRequestId = ++LoadRequestGeneration;

	TArray<FSoftObjectPath> PathsToLoad;
	if (!ClassRow.SkeletalMesh.IsNull())	PathsToLoad.AddUnique(ClassRow.SkeletalMesh.ToSoftObjectPath());
	if (!ClassRow.AnimBlueprint.IsNull())	PathsToLoad.AddUnique(ClassRow.AnimBlueprint.ToSoftObjectPath());


	auto CollectSkinPath = [&PathsToLoad](const FCharacterSkinBaseData& Row, ESkinSlot Slot, int32 SkinIndex)
		{
			if (const TArray<FSkinEntry>* SkinArray = Row.GetSkinsForSlot(Slot))
			{
				if (SkinArray->IsValidIndex(SkinIndex) && !(*SkinArray)[SkinIndex].Material.IsNull())
				{
					PathsToLoad.AddUnique((*SkinArray)[SkinIndex].Material.ToSoftObjectPath());
				}
			}
		};

	CollectSkinPath(ClassRow, ESkinSlot::Upper, InInfo.UpperBodySkinRowID);
	CollectSkinPath(ClassRow, ESkinSlot::Lower, InInfo.LowerBodySkinRowID);
	CollectSkinPath(ClassRow, ESkinSlot::Weapon, InInfo.WeaponSkinRowID);


	TWeakObjectPtr<AJ1LobbyCharacterActor> WeakThis(this);
	const ECharacterClass RequestedClass = InInfo.ClassType;

	// PathsToLoad가 비어있어도(전부 참조 없음) RequestAsyncLoad는 델리게이트를 즉시(다음 틱) 실행해준다.
	ActiveLoadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
		PathsToLoad,
		FStreamableDelegate::CreateLambda([WeakThis, ThisRequestId, RequestedClass]()
			{
				if (AJ1LobbyCharacterActor* StrongThis = WeakThis.Get())
				{
					StrongThis->HandleAssetsLoaded(ThisRequestId, RequestedClass);
				}
			})
	);
}

void AJ1LobbyCharacterActor::HandleAssetsLoaded(uint32 RequestId, ECharacterClass LoadedClassType)
{
	const FCharacterSkinBaseData* ClassRow = FindClassRow(LoadedClassType);
	if (!ClassRow)								return;

	// 로드가 끝나기 전에 InitFromServerInfo가 다시 호출되어 다른 데이터를 가리키게 됐다면
	// 이 콜백은 낡은 것이므로 화면에 반영하지 않고 무시
	if (RequestId != LoadRequestGeneration)		return;
	if (bIsEmptySlot)							return;
	
	// 스켈레톤 / 애님블루프린트 세팅
	if (USkeletalMesh* Mesh = ClassRow->SkeletalMesh.Get())		BodyMesh->SetSkeletalMesh(Mesh);
	if (UClass* AnimClass = ClassRow->AnimBlueprint.Get())		BodyMesh->SetAnimInstanceClass(AnimClass);


	// 상/하체/무기 머터리얼 적용 
	ApplySkinMaterial(*ClassRow, ESkinSlot::Upper, CachedInfo.UpperBodySkinRowID);
	ApplySkinMaterial(*ClassRow, ESkinSlot::Lower, CachedInfo.LowerBodySkinRowID);
	ApplySkinMaterial(*ClassRow, ESkinSlot::Weapon, CachedInfo.WeaponSkinRowID);

	// 정보 위젯 갱신 + 클릭 델리게이트 바인딩
	if (UJ1CharacterInfoWidget* InfoWidget = Cast<UJ1CharacterInfoWidget>(InfoWidgetComp->GetUserWidgetObject()))
	{
		InfoWidget->SetFilledData(CachedInfo.CharacterUniqueID, CachedInfo.CharacterName, CachedInfo.ClassType, CachedInfo.Level);
	}

	SetSelected(false);

	ActiveLoadHandle.Reset();
}
	

void AJ1LobbyCharacterActor::ApplySkinMaterial(const FCharacterSkinBaseData& ClassRow, ESkinSlot Slot, int32 SkinIndex)
{
	// 1) 스킨(머터리얼) 리졸브
	const TArray<FSkinEntry>* SkinArray = ClassRow.GetSkinsForSlot(Slot);
	UMaterialInterface* ResolvedMaterial = FallbackMaterial;

	if (SkinArray && SkinArray->IsValidIndex(SkinIndex))
	{
		if (UMaterialInterface* Loaded = (*SkinArray)[SkinIndex].Material.LoadSynchronous())
		{
			ResolvedMaterial = Loaded;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[LobbyCharacterActor] 스킨 인덱스 범위 초과 (Slot=%d, Index=%d) - 폴백 머터리얼 사용"),
			static_cast<int32>(Slot), SkinIndex);
	}

	if (!ResolvedMaterial)			return;
	


	// 2) 이 슬롯이 적용되어야 할 머터리얼로 교체
	const TArray<int32>* MaterialIndices = ClassRow.GetMaterialSlotIndicesForSlot(Slot);
	if (!MaterialIndices || MaterialIndices->Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LobbyCharacterActor] Slot=%d 에 매핑된 MaterialSlotIndices가 비어있습니다."),
			static_cast<int32>(Slot));
		return;
	}

	const int32 NumMaterials = BodyMesh->GetNumMaterials();
	for (int32 MaterialIdx : *MaterialIndices)
	{
		if (MaterialIdx >= 0 && MaterialIdx < NumMaterials)
		{
			BodyMesh->SetMaterial(MaterialIdx, ResolvedMaterial);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[LobbyCharacterActor] 머터리얼 인덱스(%d)가 BodyMesh 슬롯 개수(%d)를 벗어났습니다."),
				MaterialIdx, NumMaterials);
		}
	}

}

void AJ1LobbyCharacterActor::HandleInfoWidgetClicked()
{
	OnCharacterClicked.Broadcast(CachedInfo.SlotIndex);
}