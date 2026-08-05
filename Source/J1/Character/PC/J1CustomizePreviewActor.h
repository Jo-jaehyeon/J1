// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Types/J1CharacterCustomizeTypes.h"
#include "J1CustomizePreviewActor.generated.h"

UCLASS()
class J1_API AJ1CustomizePreviewActor : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AJ1CustomizePreviewActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


public:
	UFUNCTION(BlueprintCallable, Category = "Customize")
	void ApplyClass(const FCharacterSkinBaseData& ClassData);

	/** 스킨 변경 */
	UFUNCTION(BlueprintCallable, Category = "Customize")
	void ApplySkin(ESkinSlot SkinSlot, const FSkinEntry& SkinData);

	/** 슬롯 → 머터리얼 인덱스 매핑 */
	UFUNCTION(BlueprintPure, Category = "Customization")
	static int32 GetMaterialIndexForSlot(ESkinSlot Slot);


private:
	/** 비동기 로드 후 메시 적용 */
	void OnSkeletalMeshLoaded(TSoftObjectPtr<USkeletalMesh> SoftMesh, TSoftClassPtr<UAnimInstance>  SoftAnim);

	/** 비동기 로드 후 머터리얼 적용 */
	void OnMaterialLoaded(TSoftObjectPtr<UMaterialInterface> SoftMat, int32 SlotIndex);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;
};
