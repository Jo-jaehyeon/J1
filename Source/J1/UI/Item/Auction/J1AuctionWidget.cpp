// Copyright © 2026 Jerry. All rights reserved.

#include "J1AuctionWidget.h"
#include "UI/Item/Auction/J1AuctionEntryWidget.h"
#include "UI/Item/Auction/J1AuctionReceiptEntryWidget.h"
#include "UI/Item/Auction/J1AuctionPurchaseWidget.h"
#include "UI/Item/Auction/J1AuctionRegisterWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Network/Protocol/AuctionProtocol.pb.h"

void UJ1AuctionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_Purchase)	Btn_Purchase->OnClicked.AddDynamic(this, &UJ1AuctionWidget::HandlePurchaseClicked);
	if (Btn_Register)	Btn_Register->OnClicked.AddDynamic(this, &UJ1AuctionWidget::HandleRegisterClicked);
	if (Btn_Search)		Btn_Search->OnClicked.AddDynamic(this, &UJ1AuctionWidget::HandleSearchClicked);
	if (Btn_NextPage)	Btn_NextPage->OnClicked.AddDynamic(this, &UJ1AuctionWidget::HandleNextPageClicked);
	if (Btn_PrevPage)	Btn_PrevPage->OnClicked.AddDynamic(this, &UJ1AuctionWidget::HandlePrevPageClicked);
	if (Tab_Total)		Tab_Total->OnClicked.AddDynamic(this, &UJ1AuctionWidget::HandleTotalTabClicked);
	if (Tab_MyRegister) Tab_MyRegister->OnClicked.AddDynamic(this, &UJ1AuctionWidget::HandleRegisterTabClicked);
	if (Tab_Receipt)		Tab_Receipt->OnClicked.AddDynamic(this, &UJ1AuctionWidget::HandleReceiptTabClicked);

	if (PurchaseWidget) PurchaseWidget->SetVisibility(ESlateVisibility::Collapsed);
	if (RegisterWidget) RegisterWidget->SetVisibility(ESlateVisibility::Collapsed);

	UpdatePurchaseButtonEnabled();
}

void UJ1AuctionWidget::RefreshOnOpen()
{
	CurrentTab = EAuctionTab::Total;
	CurrentSearchText.Reset();
	CurrentEntries.Reset();
	CurrentMyEntries.Reset();
	CurrentReceipts.Reset();
	CurrentPageIndex = 0;
	SelectedEntryIndex = INDEX_NONE;

	if (Input_Search)	Input_Search->SetText(FText::GetEmpty());

	ApplyTabVisibility();

	// 전체 리스트와, 내 리스트 요청 (수령함은 탭을 열 때 요청)
	RequestAuctionList(false, CurrentSearchText);
	RequestAuctionList(true, CurrentSearchText);
}

void UJ1AuctionWidget::RefreshMyList()
{
	RequestAuctionList(true, FString());
}

// ─────────────────────────
//				     서버 요청&응답
// ─────────────────────────
void UJ1AuctionWidget::OnAuctionListReceived(const TArray<FAuctionEntry>& Entries)
{
	CurrentEntries = Entries; // 최대 AuctionEntriesPerBatch(100)개
	CurrentPageIndex = 0;
	SelectedEntryIndex = INDEX_NONE;

	// EntryListPanel은 세 탭이 같이 쓰므로, 다른 탭을 보고 있을 땐 데이터만 저장하고 그리지 않는다.
	if (CurrentTab != EAuctionTab::Total)		return;

	RebuildEntryList();
	UpdatePurchaseButtonEnabled();
	UpdatePageIndexText();
	UpdatePageButtonsEnabled();
}

void UJ1AuctionWidget::OnMyAuctionListReceived(const TArray<FAuctionEntry>& Entries)
{
	CurrentMyEntries = Entries;

	if (CurrentTab == EAuctionTab::MyRegister)
	{
		RebuildMyEntryList();
	}
}

void UJ1AuctionWidget::OnReceiptListReceived(const TArray<FAuctionReceiptEntry>& Receipts)
{
	CurrentReceipts = Receipts;

	if (CurrentTab == EAuctionTab::Receipt)
	{
		RebuildReceiptList();
	}
}

void UJ1AuctionWidget::HandleReceiptResult(int64 ReceiptID, bool bSuccess)
{
	const int32 ReceiptIndex = CurrentReceipts.IndexOfByPredicate([ReceiptID](const FAuctionReceiptEntry& Receipt) { return Receipt.ReceiptID == ReceiptID; });
	if (ReceiptIndex == INDEX_NONE)		return;

	if (bSuccess)
	{
		// 서버에서 이미 행이 삭제됐으므로 로컬 목록에서도 제거하고 다시 그린다.
		CurrentReceipts.RemoveAt(ReceiptIndex);
		if (CurrentTab == EAuctionTab::Receipt)		RebuildReceiptList();
		return;
	}

	// 실패 : 해당 엔트리의 버튼 잠금만 풀어서 재시도할 수 있게 한다.
	for (UJ1AuctionReceiptEntryWidget* ReceiptWidget : ReceiptEntryWidgets)
	{
		if (ReceiptWidget && ReceiptWidget->GetReceiptEntry().ReceiptID == ReceiptID)
		{
			ReceiptWidget->SetReceiptPending(false);
			break;
		}
	}
}

void UJ1AuctionWidget::RequestAuctionList(bool bmyList, const FString& InSearchText)
{
	OnRequestList.Broadcast(bmyList, InSearchText);
}

// ─────────────────────────
//					목록 렌더링
// ─────────────────────────
void UJ1AuctionWidget::SwitchTab(EAuctionTab NewTab)
{
	if (CurrentTab == NewTab)		return;

	CurrentTab = NewTab;

	// 구매 선택 상태는 탭이 바뀌면 의미가 없어짐
	SelectedEntryIndex = INDEX_NONE;
	UpdatePurchaseButtonEnabled();
	ApplyTabVisibility();

	// 이전 탭 내용이 응답 도착 전까지 남아있지 않도록 먼저 비운다.
	if (EntryListPanel)		EntryListPanel->ClearChildren();

	switch (CurrentTab)
	{
	case EAuctionTab::Total:
		if (!CurrentEntries.IsEmpty())
		{
			// 이전에 이미 받아온 목록이 있으면 재요청 없이 그대로 보여준다.
			RebuildEntryList();
			UpdatePageIndexText();
		}
		else
		{
			RequestAuctionList(false, CurrentSearchText);
		}
		break;

	case EAuctionTab::MyRegister:
		// 등록/만료/판매로 자주 바뀌는 목록이라 항상 최신으로 다시 요청.
		RequestAuctionList(true, FString());
		break;

	case EAuctionTab::Receipt:
		// 판매/구매/만료가 언제든 생길 수 있으니 탭을 열 때마다 다시 요청.
		OnRequestReceiptList.Broadcast();
		break;
	}

	UpdatePageButtonsEnabled();
}

void UJ1AuctionWidget::ApplyTabVisibility()
{
	// 구매 버튼/검색은 전체 목록 탭에서만 의미가 있음
	const ESlateVisibility BrowseOnly = (CurrentTab == EAuctionTab::Total) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	if (Btn_Purchase)	Btn_Purchase->SetVisibility(BrowseOnly);
	if (Btn_Search)		Btn_Search->SetVisibility(BrowseOnly);
	if (Input_Search)	Input_Search->SetVisibility(BrowseOnly);
	if (Txt_PageIndexs) Txt_PageIndexs->SetVisibility(BrowseOnly);

	UpdatePageButtonsEnabled();
}

void UJ1AuctionWidget::RebuildEntryList()
{
	if (!EntryListPanel || !EntryWidgetClass)
	{
		return;
	}

	EntryListPanel->ClearChildren();
	EntryWidgets.Reset();

	const int32 StartIndex = CurrentPageIndex * AuctionEntriesPerPage;
	const int32 Num = FMath::Clamp(CurrentEntries.Num() - StartIndex, 0, AuctionEntriesPerPage);

	for (int32 i = 0; i < Num; ++i)
	{
		UJ1AuctionEntryWidget* EntryWidget = CreateWidget<UJ1AuctionEntryWidget>(this, EntryWidgetClass);
		if (!EntryWidget) continue;

		EntryWidget->InitEntry(i, CurrentEntries[StartIndex + i], /*bInIsMyListing=*/ false);
		EntryWidget->OnEntryClicked.AddDynamic(this, &UJ1AuctionWidget::HandleEntryClicked);

		EntryListPanel->AddChildToVerticalBox(EntryWidget);
		EntryWidgets.Add(EntryWidget);
	}
}

void UJ1AuctionWidget::RebuildMyEntryList()
{
	if (!EntryListPanel || !EntryWidgetClass)		return;

	EntryListPanel->ClearChildren();
	MyEntryWidgets.Reset();

	// 최대 AuctionEntriesPerPage(10)개 고정이라 슬라이싱 없이 그대로 전부 보여준다.
	const int32 Num = FMath::Min(CurrentMyEntries.Num(), AuctionEntriesPerPage);
	for (int32 i = 0; i < Num; ++i)
	{
		UJ1AuctionEntryWidget* EntryWidget = CreateWidget<UJ1AuctionEntryWidget>(this, EntryWidgetClass);
		if (!EntryWidget) continue;

		EntryWidget->InitEntry(i, CurrentMyEntries[i], /*bInIsMyListing=*/ true);

		EntryListPanel->AddChildToVerticalBox(EntryWidget);
		MyEntryWidgets.Add(EntryWidget);
	}
}

void UJ1AuctionWidget::RebuildReceiptList()
{
	if (!EntryListPanel || !ReceiptEntryWidgetClass)		return;

	EntryListPanel->ClearChildren();
	ReceiptEntryWidgets.Reset();

	// 수령함은 페이지 없이 전부 보여준다. 개수가 많아질 수 있으니 EntryListPanel은 ScrollBox 안에 둘 것.
	for (const FAuctionReceiptEntry& Receipt : CurrentReceipts)
	{
		UJ1AuctionReceiptEntryWidget* ReceiptWidget = CreateWidget<UJ1AuctionReceiptEntryWidget>(this, ReceiptEntryWidgetClass);
		if (!ReceiptWidget) continue;

		ReceiptWidget->InitReceiptEntry(Receipt);
		ReceiptWidget->OnReceiptClicked.AddDynamic(this, &UJ1AuctionWidget::HandleReceiptClicked);

		EntryListPanel->AddChildToVerticalBox(ReceiptWidget);
		ReceiptEntryWidgets.Add(ReceiptWidget);
	}
}
// ─────────────────────────
//					페이지 이동
// ─────────────────────────
void UJ1AuctionWidget::GoToPage(int32 NewPageIndex)
{
	if (NewPageIndex < 0)
	{
		return;
	}

	const int32 StartIndex = NewPageIndex * AuctionEntriesPerPage;
	if (StartIndex >= CurrentEntries.Num() && NewPageIndex != 0)
	{
		return; // 이미 받아온 데이터 범위를 벗어남 - 더 없음
	}

	CurrentPageIndex = NewPageIndex;
	SelectedEntryIndex = INDEX_NONE;
	RebuildEntryList();
	UpdatePurchaseButtonEnabled();
	UpdatePageIndexText();
	UpdatePageButtonsEnabled();
}

bool UJ1AuctionWidget::CanOpenNextPage() const
{
	const int32 NextStart = (CurrentPageIndex + 1) * AuctionEntriesPerPage;
	return NextStart < CurrentEntries.Num();
}


void UJ1AuctionWidget::UpdatePageButtonsEnabled()
{
	const bool bIsBrowseTab = (CurrentTab == EAuctionTab::Total);

	// 내 등록 물품(최대 10개)/수령함은 페이지 이동이 없으므로 버튼 자체가 필요 없음.
	if (Btn_PrevPage)
	{
		Btn_PrevPage->SetVisibility(bIsBrowseTab ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		Btn_PrevPage->SetIsEnabled(bIsBrowseTab && CurrentPageIndex > 0);
	}
	if (Btn_NextPage)
	{
		Btn_NextPage->SetVisibility(bIsBrowseTab ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		Btn_NextPage->SetIsEnabled(bIsBrowseTab && CanOpenNextPage());
	}
}

// ─────────────────────────
//					 기타 UI 갱신
// ─────────────────────────
void UJ1AuctionWidget::UpdatePurchaseButtonEnabled()
{
	if (Btn_Purchase)		Btn_Purchase->SetIsEnabled(EntryWidgets.IsValidIndex(SelectedEntryIndex));
}

void UJ1AuctionWidget::UpdatePageIndexText()
{
	if (Txt_PageIndexs)		Txt_PageIndexs->SetText(FText::AsNumber(CurrentPageIndex + 1));
}


void UJ1AuctionWidget::HandleEntryClicked(int32 EntryIndex)
{
	if (!EntryWidgets.IsValidIndex(EntryIndex))		return;

	// 이전 선택 해제
	if (EntryWidgets.IsValidIndex(SelectedEntryIndex) && EntryWidgets[SelectedEntryIndex])
	{
		EntryWidgets[SelectedEntryIndex]->SetSelected(false);
	}

	SelectedEntryIndex = EntryIndex;
	EntryWidgets[SelectedEntryIndex]->SetSelected(true);

	UpdatePurchaseButtonEnabled();
}

void UJ1AuctionWidget::HandleReceiptClicked(FAuctionReceiptEntry ReceiptEntry)
{
	// 실제 수령 서버 호출은 이 델리게이트를 구독하는 쪽(PlayerController)에서.
	// 결과가 오면 HandleReceiptResult(ReceiptID, bSuccess)로 다시 알려줄 것.
	OnReceiptRequested.Broadcast(ReceiptEntry);
}

// ─────────────────────────
//				  구매/등록 패널 열기
// ─────────────────────────
void UJ1AuctionWidget::HandlePurchaseClicked()
{
	// 선택된 엔트리 없음
	if (!EntryWidgets.IsValidIndex(SelectedEntryIndex) || !PurchaseWidget)		return;

	PurchaseWidget->SetEntry(EntryWidgets[SelectedEntryIndex]->GetEntry());
	PurchaseWidget->SetVisibility(ESlateVisibility::Visible);
}

void UJ1AuctionWidget::HandleRegisterClicked()
{
	if (!RegisterWidget)	return;

	RegisterWidget->SetVisibility(ESlateVisibility::Visible);
	OnRegisterPanelOpened.Broadcast();
}

// ─────────────────────────
//				  기타 버튼 처리
// ─────────────────────────
void UJ1AuctionWidget::HandleSearchClicked()
{
	CurrentSearchText = Input_Search ? Input_Search->GetText().ToString() : FString();

	CurrentEntries.Reset();
	CurrentPageIndex = 0;

	RequestAuctionList(false, CurrentSearchText);
}

void UJ1AuctionWidget::HandleTotalTabClicked()
{
	SwitchTab(EAuctionTab::Total);
}

void UJ1AuctionWidget::HandleRegisterTabClicked()
{
	SwitchTab(EAuctionTab::MyRegister);
}

void UJ1AuctionWidget::HandleReceiptTabClicked()
{
	SwitchTab(EAuctionTab::Receipt);
}

void UJ1AuctionWidget::HandleNextPageClicked()
{
	GoToPage(CurrentPageIndex + 1);
}

void UJ1AuctionWidget::HandlePrevPageClicked()
{
	GoToPage(CurrentPageIndex - 1);
}
