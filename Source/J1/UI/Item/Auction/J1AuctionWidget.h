// Copyright © 2026 Jerry. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Types/J1AuctionDefine.h"
#include "J1AuctionWidget.generated.h"

class UJ1AuctionEntryWidget;
class UJ1AuctionPurchaseWidget;
class UJ1AuctionRegisterWidget;
class UVerticalBox;
class UButton;
class UEditableTextBox;
class UTextBlock;

// 한 페이지 최대 표시 개수
inline constexpr int32 AuctionEntriesPerPage = 10;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiptRequested, FAuctionEntry, Entry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRegisterPanelOpened);

/**
 * 경매장 메인 UI. 실제 구매 UI/등록 UI를 직접 생성하지 않고,
 * "열어달라"는 델리게이트만 쏜다 - 실제로 어떤 위젯을 어떻게 스택에 쌓을지는
 * 프로젝트에 이미 있는 UI 오케스트레이션(PlayerController의 OpenedWidget 등)에서
 * 이 델리게이트를 구독해서 처리하면 된다.
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

	// 서버 응답(배열)이 도착했을 때 호출. 네트워크 콜백 완료 지점에서 이 함수를 불러주면 된다.
	UFUNCTION(BlueprintCallable, Category = "Auction")	void OnAuctionListReceived(const TArray<FAuctionEntry>& Entries);
	UFUNCTION(BlueprintCallable, Category = "Auction")	void OnMyAuctionListReceived(const TArray<FAuctionEntry>& Entries);

private:
	UFUNCTION()	void HandlePurchaseClicked();
	UFUNCTION()	void HandleRegisterClicked();
	UFUNCTION()	void HandleSearchClicked();
	UFUNCTION()	void HandleNextPageClicked();
	UFUNCTION()	void HandlePrevPageClicked();
	UFUNCTION()	void HandleEntryClicked(int32 EntryIndex);
	UFUNCTION()	void HandleTotalTabClicked();
	UFUNCTION()	void HandleRegisterTabClicked();
	UFUNCTION()	void HandleReceiptClicked(FAuctionEntry Entry);

	// 서버에 목록을 요청. TODO: 실제 네트워크 호출로 교체.
	void RequestAuctionList(int32 InPageIndex, const FString& InSearchText);
	void RequestMyAuctionList(int32 InPageIndex);

private:
	void SwitchTab(EAuctionTab NewTab);
	void RebuildMyEntryList();

	void RebuildEntryList();
	void UpdatePurchaseButtonEnabled();
	void UpdatePageIndexText();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Auction")
	TSubclassOf<UJ1AuctionEntryWidget> EntryWidgetClass;

	// ═════════════════════
	//		 DELEGATE
	// ═════════════════════
	UPROPERTY(BlueprintAssignable, Category = "Auction")	FOnReceiptRequested		OnReceipRequested;
	UPROPERTY(BlueprintAssignable, Category = "Auction")	FOnRegisterPanelOpened	OnRegisterPanelOpened;

protected:
	// ════════════════════════════════════
	//              UMG 바인딩
	// ════════════════════════════════════

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UJ1AuctionPurchaseWidget* PurchaseWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UJ1AuctionRegisterWidget* RegisterWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UVerticalBox*	  EntryListPanel;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UEditableTextBox* Input_Search;		
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Tab_Total;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Tab_MyRegister;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Btn_Purchase;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Btn_Register;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Btn_Search;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Btn_NextPage;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UButton*		  Btn_PrevPage;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))	UTextBlock*		  Txt_PageIndexs;

private:
	UPROPERTY()TArray<UJ1AuctionEntryWidget*> MyEntryWidgets;
	UPROPERTY()TArray<UJ1AuctionEntryWidget*> EntryWidgets;

	TArray<FAuctionEntry> CurrentPageEntries;
	TArray<FAuctionEntry> CurrentMyPageEntries;

	EAuctionTab CurrentTab = EAuctionTab::Total;

	int32	CurrentPageIndex = 0;
	int32	SelectedEntryIndex = INDEX_NONE;
	FString CurrentSearchText;
};