// Copyright © 2026 Jerry. All rights reserved.

#include "J1AuctionWidget.h"
#include "UI/Item/Auction/J1AuctionEntryWidget.h"
#include "UI/Item/Auction/J1AuctionPurchaseWidget.h"
#include "UI/Item/Auction/J1AuctionRegisterWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

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

	if (PurchaseWidget) PurchaseWidget->SetVisibility(ESlateVisibility::Collapsed);
	if (RegisterWidget) RegisterWidget->SetVisibility(ESlateVisibility::Collapsed);

	UpdatePurchaseButtonEnabled();
}

void UJ1AuctionWidget::RefreshOnOpen()
{
	CurrentTab = EAuctionTab::Total;
	CurrentPageIndex = 0;
	CurrentSearchText.Reset();

	if (Input_Search)	Input_Search->SetText(FText::GetEmpty());
	
	RequestAuctionList(CurrentPageIndex, CurrentSearchText);
}

void UJ1AuctionWidget::RequestAuctionList(int32 InPageIndex, const FString& InSearchText)
{
	// TODO: 실제 서버 요청으로 교체.
	// 예) AuctionSubsystem->RequestListings(InPageIndex, J1AuctionEntriesPerPage, InSearchText,
	//         FOnAuctionListingsReceived::CreateUObject(this, &UJ1AuctionWidget::OnAuctionListReceived));
	//
	// 서버는 이름에 InSearchText가 부분 문자열로 포함된 매물만, 페이지당 최대
	// J1AuctionEntriesPerPage(10)개씩 내려준다고 가정한다.
}

void UJ1AuctionWidget::RequestMyAuctionList(int32 InPageIndex)
{
	// TODO: 실제 서버 요청으로 교체.
	// 예) AuctionSubsystem->RequestMyListings(InPageIndex, J1AuctionEntriesPerPage,
	//         FOnAuctionListingsReceived::CreateUObject(this, &UJ1AuctionWidget::OnMyAuctionListReceived));
	//
	// 서버는 각 매물의 bIsSold(판매 완료 여부)도 함께 내려준다고 가정한다.
}

void UJ1AuctionWidget::OnAuctionListReceived(const TArray<FAuctionEntry>& Entries)
{
	CurrentPageEntries = Entries;
	SelectedEntryIndex = INDEX_NONE;

	RebuildEntryList();
	UpdatePurchaseButtonEnabled();
	UpdatePageIndexText();
}

void UJ1AuctionWidget::OnMyAuctionListReceived(const TArray<FAuctionEntry>& Entries)
{
	CurrentMyPageEntries = Entries;

	RebuildMyEntryList();
	UpdatePageIndexText();
}

void UJ1AuctionWidget::SwitchTab(EAuctionTab NewTab)
{
	if (CurrentTab == NewTab)		return;
	
	CurrentTab = NewTab;
	CurrentPageIndex = 0;

	// 구매 선택 상태는 탭이 바뀌면 의미가 없어짐
	SelectedEntryIndex = INDEX_NONE;
	UpdatePurchaseButtonEnabled();

	// 구매 버튼/검색은 전체 목록 탭에서만 의미가 있음
	const bool bIsBrowseTab = (CurrentTab == EAuctionTab::Total);
	if (Btn_Purchase)	Btn_Purchase->SetVisibility(bIsBrowseTab ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (Btn_Search)		Btn_Search->SetVisibility(bIsBrowseTab ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (Input_Search)	Input_Search->SetVisibility(bIsBrowseTab ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);


	if (bIsBrowseTab)	RequestAuctionList(CurrentPageIndex, CurrentSearchText);
	else                RequestMyAuctionList(CurrentPageIndex);
}

void UJ1AuctionWidget::HandleTotalTabClicked()
{
	SwitchTab(EAuctionTab::Total);
}

void UJ1AuctionWidget::HandleRegisterTabClicked()
{
	SwitchTab(EAuctionTab::MyRegister);
}

void UJ1AuctionWidget::RebuildMyEntryList()
{
	if (!EntryListPanel || !EntryWidgetClass)		return;

	EntryListPanel->ClearChildren();
	MyEntryWidgets.Reset();

	const int32 Num = FMath::Min(CurrentMyPageEntries.Num(), AuctionEntriesPerPage);
	for (int32 i = 0; i < Num; ++i)
	{
		UJ1AuctionEntryWidget* EntryWidget = CreateWidget<UJ1AuctionEntryWidget>(this, EntryWidgetClass);
		if (!EntryWidget) continue;

		EntryWidget->InitEntry(i, CurrentMyPageEntries[i], /*bInIsMyListing=*/ true);
		EntryWidget->OnReceiptClicked.AddDynamic(this, &UJ1AuctionWidget::HandleReceiptClicked);

		EntryListPanel->AddChildToVerticalBox(EntryWidget);
		MyEntryWidgets.Add(EntryWidget);
	}
}

void UJ1AuctionWidget::HandleReceiptClicked(FAuctionEntry Entry)
{
	// TODO: 실제 수령 서버 호출은 이 델리게이트를 구독하는 쪽에서.
	// 성공 응답이 오면 해당 엔트리를 목록에서 제거하거나 RequestMyAuctionList로 다시 갱신할 것.
	OnReceipRequested.Broadcast(Entry);
}

void UJ1AuctionWidget::RebuildEntryList()
{
	if (!EntryListPanel || !EntryWidgetClass)		return;
	
	EntryListPanel->ClearChildren();
	EntryWidgets.Reset();

	const int32 Num = FMath::Min(CurrentPageEntries.Num(), AuctionEntriesPerPage);
	for (int32 i = 0; i < Num; ++i)
	{
		UJ1AuctionEntryWidget* EntryWidget = CreateWidget<UJ1AuctionEntryWidget>(this, EntryWidgetClass);
		if (!EntryWidget) continue;

		EntryWidget->InitEntry(i, CurrentPageEntries[i], /*bInIsMyListing=*/ false);
		EntryWidget->OnEntryClicked.AddDynamic(this, &UJ1AuctionWidget::HandleEntryClicked);

		EntryListPanel->AddChildToVerticalBox(EntryWidget);
		EntryWidgets.Add(EntryWidget);
	}
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

void UJ1AuctionWidget::UpdatePurchaseButtonEnabled()
{
	if (Btn_Purchase)		Btn_Purchase->SetIsEnabled(CurrentPageEntries.IsValidIndex(SelectedEntryIndex));
}

void UJ1AuctionWidget::UpdatePageIndexText()
{
	if (Txt_PageIndexs)		Txt_PageIndexs->SetText(FText::AsNumber(CurrentPageIndex + 1));
}

void UJ1AuctionWidget::HandlePurchaseClicked()
{
	// 선택된 엔트리 없음 
	if (!CurrentPageEntries.IsValidIndex(SelectedEntryIndex) || !PurchaseWidget)	return;
	
	PurchaseWidget->SetEntry(CurrentPageEntries[SelectedEntryIndex]);
	PurchaseWidget->SetVisibility(ESlateVisibility::Visible);
}

void UJ1AuctionWidget::HandleRegisterClicked()
{
	if (!RegisterWidget)	return;

	RegisterWidget->SetVisibility(ESlateVisibility::Visible);
	OnRegisterPanelOpened.Broadcast();
}

void UJ1AuctionWidget::HandleSearchClicked()
{
	CurrentPageIndex = 0;
	CurrentSearchText = Input_Search ? Input_Search->GetText().ToString() : FString();

	RequestAuctionList(CurrentPageIndex, CurrentSearchText);
}

void UJ1AuctionWidget::HandleNextPageClicked()
{
	++CurrentPageIndex;

	if (CurrentTab == EAuctionTab::Total)	RequestAuctionList(CurrentPageIndex, CurrentSearchText);
	else									RequestMyAuctionList(CurrentPageIndex);
}

void UJ1AuctionWidget::HandlePrevPageClicked()
{
	if (CurrentPageIndex <= 0)		return;

	--CurrentPageIndex;

	if (CurrentTab == EAuctionTab::Total)	RequestAuctionList(CurrentPageIndex, CurrentSearchText);
	else									RequestMyAuctionList(CurrentPageIndex);
}