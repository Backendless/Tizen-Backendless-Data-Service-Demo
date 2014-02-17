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

#ifndef _TODO_LIST_FORM_H_
#define _TODO_LIST_FORM_H_

#include <FBase.h>
#include <FSocial.h>
#include <FUi.h>

class TodoListForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::IActionEventListener
	, public Tizen::Social::ICalendarbookEventListener
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::Controls::IListViewItemEventListener
	, public Tizen::Ui::Controls::IListViewItemProvider
	, public Tizen::Ui::Scenes::ISceneEventListener
{

public:
	TodoListForm();
	virtual ~TodoListForm();

	bool Initialize(void);

public:
	virtual result OnInitializing(void);
	virtual result OnTerminating(void);
	virtual void OnActionPerformed(const Tizen::Ui::Control& source, int actionId);

	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	virtual void OnListViewItemStateChanged(Tizen::Ui::Controls::ListView& listView, int index, int elementId, Tizen::Ui::Controls::ListItemStatus status);
	virtual void OnListViewItemSwept(Tizen::Ui::Controls::ListView& listView, int index, Tizen::Ui::Controls::SweepDirection direction);
	virtual void OnListViewContextItemStateChanged(Tizen::Ui::Controls::ListView& listView, int index, int elementId, Tizen::Ui::Controls::ListContextItemStatus state);
	virtual void OnItemReordered(Tizen::Ui::Controls::ListView& view, int oldIndex, int newIndex);
	virtual int GetItemCount(void);
	virtual Tizen::Ui::Controls::ListItemBase* CreateItem(int index, int itemWidth);
	virtual bool DeleteItem(int index, Tizen::Ui::Controls::ListItemBase* pItem, int itemWidth);

	virtual void OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId& previousSceneId, const Tizen::Ui::Scenes::SceneId& currentSceneId, Tizen::Base::Collection::IList* pArgs);
	virtual void OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId& currentSceneId, const Tizen::Ui::Scenes::SceneId& nextSceneId);

	virtual void OnCalendarEventsChanged(const Tizen::Base::Collection::IList& eventChangeInfoList){}
	virtual void OnCalendarTodosChanged(const Tizen::Base::Collection::IList& todoChangeInfoList);

private:
	void GetTodoList(void);
	void LoadTodoList(void);

private:
	static const int ID_FOOTER_CREATE = 100;
	static const int ID_BUTTON_STATUS = 200;
	static const int ID_CONTEXT_STATUS_NONE = 300;
	static const int ID_CONTEXT_STATUS_NEEDS_ACTION = 301;
	static const int ID_CONTEXT_STATUS_COMPLETED = 302;
	static const int ID_CONTEXT_STATUS_IN_PROCESS = 303;
	static const int ID_CONTEXT_STATUS_CANCELLED = 304;
	static const int ID_CONTEXT_STATUS_ALL = 305;

	Tizen::Ui::Controls::Button* __pStatusContextButton;
	Tizen::Ui::Controls::ContextMenu* __pStatusContextMenu;
	Tizen::Ui::Controls::ListView* __pListView;

	Tizen::Base::Collection::IList* __pTodosList;
	Tizen::Social::Calendarbook* __pCalendarbook;
	unsigned long __selectedStatus;
	bool __loaded;
};

#endif  // _TODO_LIST_FORM_H_
