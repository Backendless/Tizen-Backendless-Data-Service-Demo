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

#include "TodoFormFactory.h"
#include "TodoListForm.h"
#include "CreateTodoForm.h"
#include "TodoDetailForm.h"
#include "EditTodoForm.h"

using namespace Tizen::Ui::Scenes;

// Definitions of extern.
const wchar_t* FORM_LIST = L"TodoListForm";
const wchar_t* FORM_DETAIL = L"TodoDetailForm";
const wchar_t* FORM_CREATION = L"CreateTodoForm";
const wchar_t* FORM_EDITION = L"EditTodoForm";

TodoFormFactory::TodoFormFactory(void)
{
}

TodoFormFactory::~TodoFormFactory(void)
{
}

Tizen::Ui::Controls::Form*
TodoFormFactory::CreateFormN(const Tizen::Base::String& formId, const Tizen::Ui::Scenes::SceneId& sceneId)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);
	Tizen::Ui::Controls::Form* pNewForm = null;

	if (formId == FORM_LIST)
	{
		TodoListForm* pForm = new (std::nothrow) TodoListForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}
	else if (formId == FORM_CREATION)
	{
		CreateTodoForm* pForm = new (std::nothrow) CreateTodoForm();
		pForm->Initialize();
		pNewForm = pForm;
	}
	else if (formId == FORM_DETAIL)
	{
		TodoDetailForm* pForm = new (std::nothrow) TodoDetailForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}
	else if (formId == FORM_EDITION)
	{
		EditTodoForm* pForm = new (std::nothrow) EditTodoForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}

	return pNewForm;
}
