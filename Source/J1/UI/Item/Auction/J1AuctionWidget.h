// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Types/J1AuctionDefine.h"
#include "J1AuctionWidget.generated.h"

class UJ1AuctionEntryWidget;
class UJ1AuctionReceiptEntryWidget;
class UJ1AuctionPurchaseWidget;
class UJ1AuctionRegisterWidget;
class UVerticalBox;
class UButton;
class UEditableTextBox;
class UTextBlock;

inline constexpr int32 AuctionEntriesPerPage = 10;													// 한 페이지 최대 표시 개수 (= 내 등록 물품 최대 개수)
inline constexpr int32 AuctionEntriesPerBatch = 100;												// 서버에 한 번 요청할 때 받아오는 최대 개수

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRequestList, bool, bmyList, FString, searchName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestReceiptList);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiptRequested, FAuctionReceiptEntry, ReceiptEntry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRegisterPanelOpened);

/**
 * 경매장 메인 UI. 탭 3개(전체 목록 / 내 등록 물품 / 수령함)를 하나의 EntryListPanel에 번갈아 그린다.
 * 구매/등록 패널은 자식 위젯(PurchaseWidget/RegisterWidget)으로 들고 Visibility만 토글한다.
 * 실제 서버 통신은 델리게이트(OnRequestList/OnRequestReceiptList/OnReceiptRequested)를 구독하는
 * PlayerController에서 처리하고, 응답은 On~Received / HandleReceiptResult로 다시 넣어준다.
 */
UCLASS()
class J1_API UJ1AuctionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

public:
	// 인벤토리와 마찬가지로, 열 때마다 호출해서 목록을 최신 상태로 갱신한다.
	UFUNCTION(BlueprintCallable, Category = "Auction")
	void RefreshOnOpen();

	// 등록 성공 등으로 내 등록 물품 목록이 바뀌었을 때 외부에서 호출.
	UFUNCTION(BlueprintCallable, Category = "Auction")
	void RefreshMyList();

	// 서버 응답(배열)이 도착했을 때 호출. 네트워크 콜백 완료 지점에서 이 함수를 불러주면 된다.
	UFUNCTION(BlueprintCallable, Category = "Auction")	void OnAuctionListReceived(const TArray<FAuctionEntry>& Entries);
	UFUNCTION(BlueprintCallable, Category = "Auction")	void OnMyAuctionListReceived(const TArray<FAuctionEntry>& Entries);
	UFUNCTION(BlueprintCallable, Category = "Auction")	void OnReceiptListReceived(const TArray<FAuctionReceiptEntry>& Receipts);

	// 수령 요청 결과. 성공이면 해당 항목을 목록에서 제거, 실패면 버튼 잠금만 해제.
	UFUNCTION(BlueprintCallable, Category = "Auction")	void HandleReceiptResult(int64 ReceiptID, bool bSuccess);

private:
	void RequestAuctionList(bool bmyList, const FString& InSearchText);

	void SwitchTab(EAuctionTab NewTab);
	void ApplyTabVisibility();
	void RebuildEntryList();
	void RebuildMyEntryList();
	void RebuildReceiptList();

	void GoToPage(int32 NewPageIndex);
	bool CanOpenNextPage() const;

	void UpdatePurchaseButtonEnabled();
	void UpdatePageIndexText();
	void UpdatePageButtonsEnabled();


	UFUNCTION()	void HandleEntryClicked(int32 EntryIndex);
	UFUNCTION()	void HandleReceiptClicked(FAuctionReceiptEntry ReceiptEntry);
	UFUNCTION()	void HandlePurchaseClicked();
	UFUNCTION()	void HandleRegisterClicked();
	UFUNCTION()	void HandleSearchClicked();
	UFUNCTION()	void HandleTotalTabClicked();
	UFUNCTION()	void HandleRegisterTabClicked();
	UFUNCTION()	void HandleReceiptTabClicked();
	UFUNCTION()	void HandleNextPageClicked();
	UFUNCTION()	void HandlePrevPageClicked();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Auction")
	TSubclassOf<UJ1AuctionEntryWidget> EntryWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Auction")
	TSubclassOf<UJ1AuctionReceiptEntryWidget> ReceiptEntryWidgetClass;

	// ═════════════════════
	//		 DELEGATE
	// ═════════════════════
	UPROPERTY(BlueprintAssignable, Category = "Auction")	FOnRequestList			OnRequestList;
	UPROPERTY(BlueprintAssignable, Category = "Auction")	FOnRequestReceiptList		OnRequestReceiptList;
	UPROPERTY(BlueprintAssignable, Category = "Auction")	FOnReceiptRequested		OnReceiptRequested;
	UPROPERTY(BlueprintAssignable, Category = "Auction")	FOnRegisterPanelOpened	OnRegisterPanelOpened;

	// ════════════════════════════════════
	//              UMG 바인딩
	// ════════════════════════════════════
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UJ1AuctionPurchaseWidget* PurchaseWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UJ1AuctionRegisterWidget* RegisterWidget;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UVerticalBox*	  EntryListPanel;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UEditableTextBox* Input_Search;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Tab_Total;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Tab_MyRegister;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Tab_Receipt;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Btn_Purchase;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Btn_Register;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Btn_Search;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Btn_PrevPage;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Btn_NextPage;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*		  Txt_PageIndexs;

private:
	UPROPERTY()	TArray<UJ1AuctionEntryWidget*>		MyEntryWidgets;
	UPROPERTY()	TArray<UJ1AuctionEntryWidget*>		EntryWidgets;
	UPROPERTY()	TArray<UJ1AuctionReceiptEntryWidget*>	ReceiptEntryWidgets;

	TArray<FAuctionEntry>		CurrentEntries;
	TArray<FAuctionEntry>		CurrentMyEntries;
	TArray<FAuctionReceiptEntry>	CurrentReceipts;

	EAuctionTab CurrentTab = EAuctionTab::Total;
	int32		CurrentPageIndex = 0;
	int32		SelectedEntryIndex = INDEX_NONE;
	FString		CurrentSearchText;
};
