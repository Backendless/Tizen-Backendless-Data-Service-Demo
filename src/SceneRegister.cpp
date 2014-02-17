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

#include "SceneRegister.h"
#include "TodoFormFactory.h"

using namespace Tizen::Ui::Scenes;

// Definitions of extern.
const wchar_t* SCENE_LIST = L"TodoListScene";
const wchar_t* SCENE_DETAIL = L"TodoDetailScene";
const wchar_t* SCENE_CREATION = L"CreateTodoScene";
const wchar_t* SCENE_EDITION = L"EditTodoScene";

SceneRegister::SceneRegister(void)
{
}

SceneRegister::~SceneRegister(void)
{
}

void
SceneRegister::RegisterAllScenes(void)
{
	static const wchar_t* PANEL_BLANK = L"";
	static TodoFormFactory formFactory;

	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);
	pSceneManager->RegisterFormFactory(formFactory);

	pSceneManager->RegisterScene(SCENE_LIST, FORM_LIST, PANEL_BLANK);
	pSceneManager->RegisterScene(SCENE_DETAIL, FORM_DETAIL, PANEL_BLANK);
	pSceneManager->RegisterScene(SCENE_CREATION, FORM_CREATION, PANEL_BLANK);
	pSceneManager->RegisterScene(SCENE_EDITION, FORM_EDITION, PANEL_BLANK);
}
