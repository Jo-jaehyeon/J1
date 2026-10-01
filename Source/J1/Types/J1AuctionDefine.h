#pragma once

#include "J1AuctionDefine.generated.h"

UENUM(BlueprintType)
enum class EAuctionTab : uint8
{
	Total,      // 전체 목록(구매용)
	MyRegister, // 내 등록 물품 (아직 안 팔린 것만)
	Receipt     // 수령함 (판매 대금 / 반환 아이템 / 구매 아이템)
};

UENUM(BlueprintType)
enum class EAuctionReceiptType : uint8
{
	SoldProceeds,   // 판매 완료 - 골드 수령
	ReturnedItem,   // 기간 만료로 안 팔림 - 아이템 반환
	PurchasedItem   // 구매 완료 - 아이템 수령
};

/**
 * 경매장에 올라온 매물 하나. 서버가 배열 형태로 내려주는 값. (DB의 auction_listings 한 행)
 * 아이콘/이름 같은 무거운 데이터는 여기 담지 않고, ItemTemplateID로 클라에서 조회한다.
 * 팔린 매물은 이 구조체로 내려오지 않는다 - 판매 대금/구매 아이템은 FAuctionReceiptEntry(수령함)로 내려온다.
 */
USTRUCT(BlueprintType)
struct FAuctionEntry
{
	GENERATED_BODY()

public:

	// 개당 가격 (장비는 Quantity가 항상 1이라 TotalPrice와 같음)
	int64 GetTotalPrice() const { return Quantity > 0 ?  Quantity * UnitPrice : 0; }

	bool IsValid() const { return ListID > INDEX_NONE && ItemTemplateID > INDEX_NONE; }

public:
	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	int64 ListID = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	int64 ItemTemplateID = INDEX_NONE;

	// 이 매물에 묶여있는 개수. 장비는 스택이 안 되니 항상 1, 소비 아이템은 여러 개 묶여있을 수 있다.
	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	int64 Quantity = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	int64 UnitPrice = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	FString ExpireAt;
};

/**
 * 수령함(내가 받아가야 할 것) 항목 하나. DB의 auction_receipts 테이블 한 행에 대응.
 * ReceiptType에 따라 골드(SoldProceeds)를 줄지 아이템(ReturnedItem/PurchasedItem)을 줄지 갈린다.
 * 수령에 성공하면 서버는 해당 행을 바로 삭제한다.
 */
USTRUCT(BlueprintType)
struct FAuctionReceiptEntry
{
	GENERATED_BODY()

public:
	bool IsGoldReceipt() const { return ReceiptType == EAuctionReceiptType::SoldProceeds; }
	bool IsValid() const { return ReceiptID > INDEX_NONE; }

public:
	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	int64 ReceiptID = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	EAuctionReceiptType ReceiptType = EAuctionReceiptType::SoldProceeds;

	// ReceiptType == SoldProceeds일 때만 의미 있음
	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	int64 GoldAmount = 0;

	// ReceiptType == ReturnedItem / PurchasedItem일 때만 의미 있음
	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	int64 ItemTemplateID = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Auction")
	int64 Quantity = 0;
};
