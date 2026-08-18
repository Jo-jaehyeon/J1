// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "J1ItemFragment.generated.h"

class UJ1ItemInstance;

/**
 * 아이템 템플릿에 꽂아넣는 기능 조각. 무기/방어구/유틸리티 등은 이 클래스를 상속해서
 * 각자 필요한 데이터와 OnInstanceCreated 시점 로직만 추가한다.
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UJ1ItemFragment : public UObject
{
	GENERATED_BODY()

public:
	// 이 프래그먼트를 가진 템플릿으로 ItemInstance가 생성될 때 호출된다.
	// (ex: 희귀도에 따른 스탯 태그 부여)
	virtual void OnInstanceCreated(UJ1ItemInstance* Instance) const {}
};