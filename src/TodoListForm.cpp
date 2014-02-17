//
// Copyright (c) 2012 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.1 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <new>
#include <FApp.h>

#include "TodoListForm.h"
#include "SceneRegister.h"
#include "Backendless.h"

using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Graphics;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Social;

static const int LIST_HEIGHT = 112;
static const int BUTTON_HEIGHT = 74;

TodoListForm::TodoListForm()
	: __pTodosList(null)
	, __pCalendarbook(null)
	, __selectedStatus(TODO_STATUS_ALL)
	, __loaded(false)
{
}

TodoListForm::~TodoListForm()
{
}

bool
TodoListForm::Initialize(void)
{
	result r = E_SUCCESS;

	__pCalendarbook = new Calendarbook();
	TryReturn(__pCalendarbook != null, false, "Failed to instantiate calendarbook.");

	r = __pCalendarbook->Construct(*this);
	if(IsFailed(r))
	{
		__pCalendarbook->Construct();
		__pCalendarbook->GetLatestVersion();
		r = GetLastResult();

		MessageBox messageBox;

		if(r == E_USER_NOT_CONSENTED)
		{
			messageBox.Construct(L"Error", L"The calendar privacy should be enabled.", MSGBOX_STYLE_OK);
		}
		else
		{
			messageBox.Construct(L"Error", L"Failed to construct the calendarbook.", MSGBOX_STYLE_OK);
		}

		int doModal;
		messageBox.ShowAndWait(doModal);

		UiApp* pApp = UiApp::GetInstance();
		AppAssert(pApp);
		pApp->Terminate();
	}

	r = Construct(FORM_STYLE_NORMAL | FORM_STYLE_FOOTER | FORM_STYLE_HEADER | FORM_STYLE_PORTRAIT_INDICATOR);
	TryReturn(!IsFailed(r), false, "[%s] Failed to construct the form.", GetErrorMessage(r));

	return true;
}

result
TodoListForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	Header* pHeader = GetHeader();
	AppAssert(pHeader);
	pHeader->SetStyle(HEADER_STYLE_TITLE);
	pHeader->SetTitleText(L"Todo");

	Footer* pFooter = GetFooter();
	AppAssert(pFooter);
	pFooter->SetStyle(FOOTER_STYLE_BUTTON_TEXT);

	FooterItem footerCreate;
	footerCreate.Construct(ID_FOOTER_CREATE);
	footerCreate.SetText(L"Create");
	pFooter->AddItem(footerCreate);
	pFooter->AddActionEventListener(*this);

	SetFormBackEventListener(this);

	static const int UI_POSITION_GAP = 0;

	__pStatusContextButton = new (std::nothrow) Button();
	__pStatusContextButton->Construct(Rectangle(GetClientAreaBounds().width * 2 / 3, UI_POSITION_GAP, GetClientAreaBounds().width / 3, BUTTON_HEIGHT), L"All");
	__pStatusContextButton->SetActionId(ID_BUTTON_STATUS);
	__pStatusContextButton->AddActionEventListener(*this);
	AddControl(__pStatusContextButton);

	__pStatusContextMenu = new (std::nothrow) ContextMenu();
	__pStatusContextMenu->Construct(Point(GetClientAreaBounds().width * 5 / 6, BUTTON_HEIGHT * 3), CONTEXT_MENU_STYLE_LIST);
	__pStatusContextMenu->AddItem(L"All", ID_CONTEXT_STATUS_ALL);
	__pStatusContextMenu->AddItem(L"None", ID_CONTEXT_STATUS_NONE);
	__pStatusContextMenu->AddItem(L"Needs action", ID_CONTEXT_STATUS_NEEDS_ACTION);
	__pStatusContextMenu->AddItem(L"Completed", ID_CONTEXT_STATUS_COMPLETED);
	__pStatusContextMenu->AddItem(L"In process", ID_CONTEXT_STATUS_IN_PROCESS);
	__pStatusContextMenu->AddItem(L"Cancelled", ID_CONTEXT_STATUS_CANCELLED);
	__pStatusContextMenu->AddActionEventListener(*this);

	__pListView = new (std::nothrow) ListView();
	__pListView->Construct(Rectangle(UI_POSITION_GAP, BUTTON_HEIGHT, GetClientAreaBounds().width, GetClientAreaBounds().height - BUTTON_HEIGHT));
	__pListView->SetTextOfEmptyList(L"(No todo)");
	__pListView->SetItemProvider(*this);
	__pListView->AddListViewItemEventListener(*this);
	AddControl(__pListView);

	__selectedStatus = TODO_STATUS_ALL;

	return r;
}

result
TodoListForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	delete __pCalendarbook;

	if (__pTodosList != null)
	{
		__pTodosList->RemoveAll(true);
		delete __pTodosList;
	}

	__pStatusContextMenu->Destroy();

	return r;
}

void
TodoListForm::OnActionPerformed(const Tizen::Ui::Control& source, int actionId)
{

	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	switch (actionId)
	{
	case ID_FOOTER_CREATE:
		pSceneManager->GoForward(ForwardSceneTransition(SCENE_CREATION));
		break;

	case ID_BUTTON_STATUS:
		__pStatusContextMenu->SetFocusable(true);
		__pStatusContextMenu->SetShowState(true);
		__pStatusContextMenu->Show();
		break;

	case ID_CONTEXT_STATUS_NONE:
		__pStatusContextButton->SetText(L"None");
		__pStatusContextButton->Invalidate(false);
		__selectedStatus = TODO_STATUS_NONE;

		GetTodoList();
		__pListView->UpdateList();
		break;

	case ID_CONTEXT_STATUS_NEEDS_ACTION:
		__pStatusContextButton->SetText(L"Needs action");
		__pStatusContextButton->Invalidate(false);
		__selectedStatus = TODO_STATUS_NEEDS_ACTION;

		GetTodoList();
		__pListView->UpdateList();
		break;

	case ID_CONTEXT_STATUS_COMPLETED:
		__pStatusContextButton->SetText(L"Completed");
		__pStatusContextButton->Invalidate(false);
		__selectedStatus = TODO_STATUS_COMPLETED;

		GetTodoList();
		__pListView->UpdateList();
		break;

	case ID_CONTEXT_STATUS_IN_PROCESS:
		__pStatusContextButton->SetText(L"In process");
		__pStatusContextButton->Invalidate(false);
		__selectedStatus = TODO_STATUS_IN_PROCESS;

		GetTodoList();
		__pListView->UpdateList();
		break;

	case ID_CONTEXT_STATUS_CANCELLED:
		__pStatusContextButton->SetText(L"Cancelled");
		__pStatusContextButton->Invalidate(false);
		__selectedStatus = TODO_STATUS_CANCELLED;

		GetTodoList();
		__pListView->UpdateList();
		break;

	case ID_CONTEXT_STATUS_ALL:
		__pStatusContextButton->SetText(L"All");
		__pStatusContextButton->Invalidate(false);
		__selectedStatus = TODO_STATUS_ALL;

		GetTodoList();
		__pListView->UpdateList();

		__loaded = false;
		LoadTodoList();
		break;

	default:
		break;
	}
}

void
TodoListForm::OnFormBackRequested(Tizen::Ui::Controls::Form& source)
{
	UiApp* pApp = UiApp::GetInstance();
	AppAssert(pApp);
	pApp->Terminate();
}

void
TodoListForm::OnListViewItemStateChanged(Tizen::Ui::Controls::ListView& listView, int index, int elementId, Tizen::Ui::Controls::ListItemStatus status)
{
	if (status == LIST_ITEM_STATUS_SELECTED)
	{
		SceneManager* pSceneManager = SceneManager::GetInstance();
		AppAssert(pSceneManager);

		CalTodo* pTodo = static_cast< CalTodo* >(__pTodosList->GetAt(index));
		AppAssert(pTodo);

		ArrayList* pList = new (std::nothrow) ArrayList();
		pList->Construct();
		pList->Add(*new (std::nothrow) Integer(pTodo->GetRecordId()));

		pSceneManager->GoForward(ForwardSceneTransition(SCENE_DETAIL), pList);
	}
}

void
TodoListForm::OnListViewItemSwept(Tizen::Ui::Controls::ListView& listView, int index, Tizen::Ui::Controls::SweepDirection direction)
{
}

void
TodoListForm::OnListViewContextItemStateChanged(Tizen::Ui::Controls::ListView& listView, int index, int elementId, Tizen::Ui::Controls::ListContextItemStatus state)
{
}

void
TodoListForm::OnItemReordered(Tizen::Ui::Controls::ListView& view, int oldIndex, int newIndex)
{
}

int
TodoListForm::GetItemCount(void)
{
	if (__pTodosList)
	{
		return __pTodosList->GetCount();
	}
	else
	{
		return 0;
	}
}

Tizen::Ui::Controls::ListItemBase*
TodoListForm::CreateItem(int index, int itemWidth)
{

	SimpleItem* pItem = new SimpleItem();
	AppAssert(pItem);

	CalTodo* pTodo = null;

	pItem->Construct(Dimension(itemWidth, LIST_HEIGHT), LIST_ANNEX_STYLE_NORMAL);
	pTodo = static_cast< CalTodo* >(__pTodosList->GetAt(index));

	String listItemString;
	String subject = pTodo->GetSubject();

	if (subject.IsEmpty())
	{
		subject = L"(No subject)";
	}

	listItemString.Append(subject);
	pItem->SetElement(listItemString);

	return pItem;
}

bool
TodoListForm::DeleteItem(int index, Tizen::Ui::Controls::ListItemBase* pItem, int itemWidth)
{
	delete pItem;
	return true;
}

void
TodoListForm::OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId& previousSceneId, const Tizen::Ui::Scenes::SceneId& currentSceneId, Tizen::Base::Collection::IList* pArgs)
{
	AppLog(">>>>> TodoListForm::OnSceneActivatedN");

	GetTodoList();
	__pListView->UpdateList();

	LoadTodoList();
}

void
TodoListForm::OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId& currentSceneId, const Tizen::Ui::Scenes::SceneId& nextSceneId)
{
}

void
TodoListForm::OnCalendarTodosChanged(const Tizen::Base::Collection::IList& todoChangeInfoList)
{
	AppLog(">>>>> TodoListForm::OnCalendarTodosChanged");

	GetTodoList();
	__pListView->UpdateList();
}

void
TodoListForm::GetTodoList(void)
{
	result r = E_SUCCESS;

	int requestEnum = Backendless::GetInstance()->GetCurrentRequestEnum();
	if (requestEnum != 0)
	{
		AppLogException("GetTodoList [requestEnum = %d] Blocked to get the todo list.", requestEnum);
		return;
	}

	if (__pTodosList)
	{
		__pTodosList->RemoveAll(true);
		delete __pTodosList;
		__pTodosList = null;
	}

	CalendarbookFilter filter(CB_FI_TYPE_TODO);

	if (__selectedStatus != TODO_STATUS_ALL)
	{
		filter.AppendInt(FI_CONJ_OP_NONE, TODO_FI_PR_STATUS, FI_CMP_OP_EQUAL, __selectedStatus);
	}

	__pTodosList = __pCalendarbook->SearchN(filter, TODO_FI_PR_DUE_DATE, SORT_ORDER_ASCENDING);
	r = GetLastResult();
	if(IsFailed(r))
	{
		MessageBox messageBox;
		messageBox.Construct(L"Error", "Failed to get the todo list", MSGBOX_STYLE_OK, 0);
		int doModal;
		messageBox.ShowAndWait(doModal);

		AppLogException("[%s] Failed to get the todo list.", GetErrorMessage(r));
	}

	Backendless::GetInstance()->SetExternalProperties(__pCalendarbook, __pTodosList, __pListView);
}

void
TodoListForm::LoadTodoList(void)
{

	if (__loaded)
		return;

	__loaded = true;

	// clear CalendarBook
	CalTodo* pTodo = null;
	int size = __pTodosList->GetCount();
	for (int i = 0; i < size; i++)
	{
		pTodo = static_cast< CalTodo* >(__pTodosList->GetAt(i));
		RecordId todoId = pTodo->GetRecordId();
		result r = __pCalendarbook->RemoveTodo(todoId);
		if (IsFailed(r))
		{
			AppLogException("RequestLoadTodosList() Calendarbook->RemoveTodo(%d) failed. (%s)", todoId , GetErrorMessage(r));
		}
	}

	__pTodosList->RemoveAll(true);

	// **** BACKENDLESS LOAD LIST (__pTodosList) ************************************
	result r = Backendless::GetInstance()->RequestLoadTodosList();
	if(IsFailed(r))
		AppLogException("[%s] Failed backendless LOAD LIST.", GetErrorMessage(r));
	// ******************************************************************************
}
