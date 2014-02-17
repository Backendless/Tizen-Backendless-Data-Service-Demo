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
#include <FLocales.h>

#include "TodoDetailForm.h"
#include "SceneRegister.h"
#include "Backendless.h"

using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Graphics;
using namespace Tizen::Locales;
using namespace Tizen::Social;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;

TodoDetailForm::TodoDetailForm()
	: __pCalendarbook(null)
	, __pTodo(null)
{
}

TodoDetailForm::~TodoDetailForm()
{
}

bool
TodoDetailForm::Initialize(void)
{
	result r = E_SUCCESS;

	__pCalendarbook = new Calendarbook();
	TryReturn(__pCalendarbook != null, false, "Failed to instantiate calendarbook.");

	r = __pCalendarbook->Construct();
	TryReturn(r == E_SUCCESS, false, "[%s] Failed to construct the calendarbook.", GetErrorMessage(r));

	Construct(FORM_STYLE_NORMAL | FORM_STYLE_FOOTER | FORM_STYLE_HEADER | FORM_STYLE_PORTRAIT_INDICATOR);
	TryReturn(r == E_SUCCESS, false, "[%s] Failed to construct the form.", GetErrorMessage(r));

	return true;
}

result
TodoDetailForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	Header* pHeader = GetHeader();
	AppAssert(pHeader);
	pHeader->SetStyle(HEADER_STYLE_TITLE);
	pHeader->SetTitleText(L"Details");

	Footer* pFooter = GetFooter();
	AppAssert(pFooter);
	pFooter->SetStyle(FOOTER_STYLE_BUTTON_TEXT);

	FooterItem footerEdit;
	footerEdit.Construct(ID_FOOTER_EDIT);
	footerEdit.SetText(L"Edit");
	pFooter->AddItem(footerEdit);

	FooterItem footerDelete;
	footerDelete.Construct(ID_FOOTER_DELETE);
	footerDelete.SetText(L"Delete");
	pFooter->AddItem(footerDelete);
	pFooter->AddActionEventListener(*this);

	SetFormBackEventListener(this);

	static const unsigned int COLOR_BACKGROUND_LABEL = 0xFFEFEDE5;
	static const unsigned int COLOR_TITLE_LABEL = 0xFF808080;

	static const int UI_X_POSITION_GAP = 20;
	static const int UI_WIDTH = GetClientAreaBounds().width - 40;
	static const int UI_X_POSITION_MIDDLE = UI_WIDTH / 4 + UI_X_POSITION_GAP;
	static const int UI_HEIGHT = 112;
	static const int UI_SPACE = 26;
	int yPosition = 0;

	ScrollPanel* pScrollPanel = new (std::nothrow) ScrollPanel();
	pScrollPanel->Construct(Rectangle(0, 0, GetClientAreaBounds().width, GetClientAreaBounds().height));

	String date;
	DateTime displayStartDate;
	DateTime displayDueDate;

	// Subject
	Label* pSubjectLabel = new (std::nothrow) Label();
	pSubjectLabel->Construct(Rectangle(UI_X_POSITION_GAP, yPosition, UI_WIDTH, UI_HEIGHT), L"Subject");
	pSubjectLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pSubjectLabel->SetTextColor(COLOR_TITLE_LABEL);
	pSubjectLabel->SetBackgroundColor(Color(COLOR_BACKGROUND_LABEL));
	pScrollPanel->AddControl(pSubjectLabel);

	__pSubjectLabelData = new (std::nothrow) Label();
	__pSubjectLabelData->Construct(Rectangle(UI_X_POSITION_MIDDLE, yPosition, UI_WIDTH * 3 / 4, UI_HEIGHT), L"");
	__pSubjectLabelData->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pScrollPanel->AddControl(__pSubjectLabelData);

	// Start Date
	Label* pStartDateLabel = new (std::nothrow) Label();
	pStartDateLabel->Construct(Rectangle(UI_X_POSITION_GAP, yPosition += UI_HEIGHT + UI_SPACE, UI_WIDTH, UI_HEIGHT), L"Start");
	pStartDateLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pStartDateLabel->SetTextColor(COLOR_TITLE_LABEL);
	pStartDateLabel->SetBackgroundColor(Color(COLOR_BACKGROUND_LABEL));
	pScrollPanel->AddControl(pStartDateLabel);

	__pStartDateLabelData = new (std::nothrow) Label();
	__pStartDateLabelData->Construct(Rectangle(UI_X_POSITION_MIDDLE, yPosition, UI_WIDTH * 3 / 4, UI_HEIGHT), L"");
	__pStartDateLabelData->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pScrollPanel->AddControl(__pStartDateLabelData);

	// Due Date
	Label* pDueDateLabel = new (std::nothrow) Label();
	pDueDateLabel->Construct(Rectangle(UI_X_POSITION_GAP, yPosition += UI_HEIGHT, UI_WIDTH, UI_HEIGHT), L"Due");
	pDueDateLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pDueDateLabel->SetTextColor(COLOR_TITLE_LABEL);
	pDueDateLabel->SetBackgroundColor(Color(COLOR_BACKGROUND_LABEL));
	pScrollPanel->AddControl(pDueDateLabel);

	__pDueDateLabelData = new (std::nothrow) Label();
	__pDueDateLabelData->Construct(Rectangle(UI_X_POSITION_MIDDLE, yPosition, UI_WIDTH * 3 / 4, UI_HEIGHT), L"");
	__pDueDateLabelData->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pScrollPanel->AddControl(__pDueDateLabelData);

	// Location
	Label* pLocationLabel = new (std::nothrow) Label();
	pLocationLabel->Construct(Rectangle(UI_X_POSITION_GAP, yPosition += UI_HEIGHT + UI_SPACE, UI_WIDTH, UI_HEIGHT), L"Location");
	pLocationLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pLocationLabel->SetTextColor(COLOR_TITLE_LABEL);
	pLocationLabel->SetBackgroundColor(Color(COLOR_BACKGROUND_LABEL));
	pScrollPanel->AddControl(pLocationLabel);

	__pLocationLabelData = new (std::nothrow) Label();
	__pLocationLabelData->Construct(Rectangle(UI_X_POSITION_MIDDLE, yPosition, UI_WIDTH * 3 / 4, UI_HEIGHT), L"");
	__pLocationLabelData->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pScrollPanel->AddControl(__pLocationLabelData);

	// Priority
	Label* pPriorityLabel = new (std::nothrow) Label();
	pPriorityLabel->Construct(Rectangle(UI_X_POSITION_GAP, yPosition += UI_HEIGHT + UI_SPACE, UI_WIDTH, UI_HEIGHT), L"Priority");
	pPriorityLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pPriorityLabel->SetTextColor(COLOR_TITLE_LABEL);
	pPriorityLabel->SetBackgroundColor(Color(COLOR_BACKGROUND_LABEL));
	pScrollPanel->AddControl(pPriorityLabel);

	__pPriorityLabelData = new (std::nothrow) Label();
	__pPriorityLabelData->Construct(Rectangle(UI_X_POSITION_MIDDLE, yPosition, UI_WIDTH * 3 / 4, UI_HEIGHT), L"");
	__pPriorityLabelData->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pScrollPanel->AddControl(__pPriorityLabelData);

	// Sensitivity
	Label* pSensitivityLabel = new (std::nothrow) Label();
	pSensitivityLabel->Construct(Rectangle(UI_X_POSITION_GAP, yPosition += UI_HEIGHT, UI_WIDTH, UI_HEIGHT), L"Sensitivity");
	pSensitivityLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pSensitivityLabel->SetTextColor(COLOR_TITLE_LABEL);
	pSensitivityLabel->SetBackgroundColor(Color(COLOR_BACKGROUND_LABEL));
	pScrollPanel->AddControl(pSensitivityLabel);

	__pSensitivityLabelData = new (std::nothrow) Label();
	__pSensitivityLabelData->Construct(Rectangle(UI_X_POSITION_MIDDLE, yPosition, UI_WIDTH * 3 / 4, UI_HEIGHT), L"");
	__pSensitivityLabelData->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pScrollPanel->AddControl(__pSensitivityLabelData);

	// Status
	Label* pStatusLabel = new (std::nothrow) Label();
	pStatusLabel->Construct(Rectangle(UI_X_POSITION_GAP, yPosition += UI_HEIGHT, UI_WIDTH, UI_HEIGHT), L"Status");
	pStatusLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pStatusLabel->SetTextColor(COLOR_TITLE_LABEL);
	pStatusLabel->SetBackgroundColor(Color(COLOR_BACKGROUND_LABEL));
	pScrollPanel->AddControl(pStatusLabel);

	__pStatusLabelData = new (std::nothrow) Label();
	__pStatusLabelData->Construct(Rectangle(UI_X_POSITION_MIDDLE, yPosition, UI_WIDTH * 3 / 4, UI_HEIGHT), L"");
	__pStatusLabelData->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pScrollPanel->AddControl(__pStatusLabelData);

	// Description
	Label* pDescriptionLabel = new (std::nothrow) Label();
	pDescriptionLabel->Construct(Rectangle(UI_X_POSITION_GAP, yPosition += UI_HEIGHT + UI_SPACE, UI_WIDTH, UI_HEIGHT), L"Description");
	pDescriptionLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pDescriptionLabel->SetTextColor(COLOR_TITLE_LABEL);
	pDescriptionLabel->SetBackgroundColor(Color(COLOR_BACKGROUND_LABEL));
	pScrollPanel->AddControl(pDescriptionLabel);

	__pDescriptionLabelData = new (std::nothrow) Label();
	__pDescriptionLabelData->Construct(Rectangle(UI_X_POSITION_MIDDLE, yPosition, UI_WIDTH * 3 / 4, UI_HEIGHT), L"");
	__pDescriptionLabelData->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pScrollPanel->AddControl(__pDescriptionLabelData);

	AddControl(pScrollPanel);

	return r;
}

result
TodoDetailForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	delete __pCalendarbook;

	if (__pTodo != null)
	{
		delete __pTodo;
	}

	return r;
}

void
TodoDetailForm::OnActionPerformed(const Tizen::Ui::Control& source, int actionId)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	switch (actionId)
	{
	case ID_FOOTER_EDIT:
	{
		if (__pTodo != null)
		{
			ArrayList* pList = new ArrayList();
			pList->Construct();
			pList->Add(*(new (std::nothrow) CalTodo(*__pTodo)));

			pSceneManager->GoForward(ForwardSceneTransition(SCENE_EDITION), pList);
		}
		else
		{
			MessageBox messageBox;
			messageBox.Construct(L"Error", "Failed to edit todo", MSGBOX_STYLE_OK, 0);
			int doModal;
			messageBox.ShowAndWait(doModal);
		}
	}
	break;

	case ID_FOOTER_DELETE:
	{
		DeleteTodo();
	}
	break;

	default:
	{
	}
	break;
	}
}

void
TodoDetailForm::OnFormBackRequested(Tizen::Ui::Controls::Form& source)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	pSceneManager->GoBackward(BackwardSceneTransition(SCENE_LIST));
}

void
TodoDetailForm::OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId& previousSceneId, const Tizen::Ui::Scenes::SceneId& currentSceneId, Tizen::Base::Collection::IList* pArgs)
{
	result r = E_SUCCESS;

	if (pArgs != null)
	{
		Integer* pId = static_cast< Integer* >(pArgs->GetAt(0));
		RecordId todoId = pId->ToInt();
		r = GetTodo(todoId);

		if (IsFailed(r))
		{
			MessageBox messageBox;
			messageBox.Construct(L"Error", "Failed to get todo", MSGBOX_STYLE_OK, 0);
			int doModal;
			messageBox.ShowAndWait(doModal);

			AppLogException("[%s] Failed to get todo.", GetErrorMessage(r));
		}
		else
		{
			UpdateTodoView();
			Invalidate(true);
		}

		pArgs->RemoveAll(true);
		delete pArgs;
	}
}

void
TodoDetailForm::OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId& currentSceneId, const Tizen::Ui::Scenes::SceneId& nextSceneId)
{
}

result
TodoDetailForm::GetTodo(Tizen::Social::RecordId todoId)
{
	result r = E_SUCCESS;
	if (__pTodo)
	{
		delete __pTodo;
		__pTodo = null;
	}
	__pTodo = __pCalendarbook->GetTodoN(todoId);

	r = GetLastResult();
	TryReturn(!IsFailed(r), r, "[%s] Failed to get the Todo.", GetErrorMessage(r));
	return r;
}

void
TodoDetailForm::UpdateTodoView()
{
	String propertyValue;

	// Subject
	propertyValue = __pTodo->GetSubject();
	if (propertyValue.IsEmpty())
	{
		propertyValue = L"(No subject)";
	}
	__pSubjectLabelData->SetText(propertyValue);


	// Convert UTC time to local time
	DateTime startDate = __pTodo->GetStartDate();
	DateTime dueDate = __pTodo->GetDueDate();

	LocaleManager localeManager;
	localeManager.Construct();

	TimeZone timeZone = localeManager.GetSystemTimeZone();
	startDate = timeZone.UtcTimeToWallTime(startDate);
	dueDate = timeZone.UtcTimeToWallTime(dueDate);

	// Start Date
	propertyValue = startDate.ToString();
	__pStartDateLabelData->SetText(propertyValue);

	// Due Date
	propertyValue = dueDate.ToString();
	__pDueDateLabelData->SetText(propertyValue);

	// Location
	propertyValue = __pTodo->GetLocation();

	if (propertyValue.IsEmpty())
	{
		propertyValue = L"(No location)";
	}
	__pLocationLabelData->SetText(propertyValue);

	// Priority
	switch (__pTodo->GetPriority())
	{
	case TODO_PRIORITY_LOW:
		__pPriorityLabelData->SetText(L"Low");
		break;

	case TODO_PRIORITY_NORMAL:
		__pPriorityLabelData->SetText(L"Normal");
		break;

	case TODO_PRIORITY_HIGH:
		__pPriorityLabelData->SetText(L"High");
		break;

	default:
		break;
	}

	// Sensitivity
	switch (__pTodo->GetSensitivity())
	{
	case SENSITIVITY_PUBLIC:
		__pSensitivityLabelData->SetText(L"Public");
		break;

	case SENSITIVITY_PRIVATE:
		__pSensitivityLabelData->SetText(L"Private");
		break;

	case SENSITIVITY_CONFIDENTIAL:
		__pSensitivityLabelData->SetText(L"Confidential");
		break;

	default:
		break;
	}

	// Status
	switch (__pTodo->GetStatus())
	{
	case TODO_STATUS_NONE:
		__pStatusLabelData->SetText(L"None");
		break;

	case TODO_STATUS_COMPLETED:
		__pStatusLabelData->SetText(L"Completed");
		break;

	case TODO_STATUS_NEEDS_ACTION:
		__pStatusLabelData->SetText(L"Needs action");
		break;

	case TODO_STATUS_IN_PROCESS:
		__pStatusLabelData->SetText(L"In process");
		break;

	case TODO_STATUS_CANCELLED:
		__pStatusLabelData->SetText(L"Cancelled");
		break;

	default:
		break;
	}

	// Description
	propertyValue = __pTodo->GetDescription();
	if (propertyValue.IsEmpty())
	{
		propertyValue = L"(No description)";
	}
	__pDescriptionLabelData->SetText(propertyValue);
}

void
TodoDetailForm::DeleteTodo(void)
{

	AppLog("TodoDetailForm::DeleteTodo");

	result r = E_SUCCESS;

	if(__pTodo != null)
	{
		MessageBox messageBox;
		r = messageBox.Construct(L"Delete todo", "Are you sure you want to delete?", MSGBOX_STYLE_OKCANCEL, 0);
		TryReturnVoid(!IsFailed(r), "[%s] Failed to construct messageBox.", GetErrorMessage(r));

		int modalResult;
		messageBox.ShowAndWait(modalResult);

		if (modalResult == MSGBOX_RESULT_OK)
		{
			r = __pCalendarbook->RemoveTodo(__pTodo->GetRecordId());

			if (IsFailed(r))
			{
				MessageBox messageBox;
				messageBox.Construct(L"Error", "Failed to delete todo", MSGBOX_STYLE_OK, 0);
				int doModal;
				messageBox.ShowAndWait(doModal);

				AppLogException("[%s] Failed to delete todo.", GetErrorMessage(r));
			}
			else
			{
				// **** BACKENDLESS DELETE (__pTodo) ********************************************
				r = Backendless::GetInstance()->RequestDeleteTodo(*__pTodo);
				if (IsFailed(r))
					AppLogException("[%s] Failed backendless DELETE.", GetErrorMessage(r));
				// ******************************************************************************

				SceneManager* pSceneManager = SceneManager::GetInstance();
				AppAssert(pSceneManager);

				pSceneManager->GoBackward(BackwardSceneTransition(SCENE_LIST));
			}
		}
	}
	else
	{
		MessageBox messageBox;
		messageBox.Construct(L"Error", "Failed to delete todo", MSGBOX_STYLE_OK, 0);
		int doModal;
		messageBox.ShowAndWait(doModal);
	}

}
