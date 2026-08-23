#pragma once

#include "J1AuctionDefine.generated.h"

/**
 * 경매장에 올라온 매물 하나. 서버가 배열 형태로 내려주는 값.
 * 아이콘/이름 같은 무거운 데이터는 여기 담지 않고, ItemTemplateID로
 */
USTRUCT(BlueprintType)
struct FAuctionEntry
{
	GENERATED_BODY()

public:

	// 개당 가격 (장비는 Quantity가 항상 1이라 TotalPrice와 같음)
	int64 GetPricePerUnit() const { return Quantity > 0 ? (TotalPrice / Quantity) : TotalPrice; }

	bool IsValid() const { return ListID > INDEX_NONE && ItemTemplateID > INDEX_NONE; }

public:
	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	int32 ListID = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	int32 ItemTemplateID = INDEX_NONE;

	// 이 매물에 묶여있는 개수. 장비는 스택이 안 되니 항상 1, 소비 아이템은 여러 개 묶여있을 수 있다.
	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	int32 Quantity = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	int64 TotalPrice = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	FDateTime ExpireAt;

	// "내 판매 목록" 탭에서만 의미 있는 값. 전체 목록(구매용) 엔트리에서는 항상 false로 무시됨.
	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	bool bIsSold = false;
};

UENUM(BlueprintType)
enum class EAuctionTab : uint8
{
	Total,      // 전체 목록(구매용)
	MyRegister   // 내 판매 목록
};