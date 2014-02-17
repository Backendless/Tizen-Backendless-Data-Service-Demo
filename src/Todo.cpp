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

#include "Todo.h"
#include "TodoFrame.h"
#include "TodoFormFactory.h"
#include "SceneRegister.h"

using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::System;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;


TodoApp::TodoApp(void)
{
}

TodoApp::~TodoApp(void)
{
}

UiApp*
TodoApp::CreateInstance(void)
{
	// Create the instance through the constructor.
	return new TodoApp();
}

bool
TodoApp::OnAppInitializing(AppRegistry& appRegistry)
{
	return true;
}

bool
TodoApp::OnAppInitialized(void)
{
	result r = E_SUCCESS;

	// Create a Frame
	TodoFrame* pTodoFrame = new TodoFrame();
	pTodoFrame->Construct();
	pTodoFrame->SetName(L"Todo");
	AddFrame(*pTodoFrame);

	// SceneManagement initializing
	SceneRegister::RegisterAllScenes();

	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	r = pSceneManager->GoForward(ForwardSceneTransition(SCENE_LIST));
	TryReturn(!IsFailed(r), false, "%s", GetErrorMessage(r));

	return true;
}

bool
TodoApp::OnAppWillTerminate(void)
{
	return true;
}

bool
TodoApp::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
	return true;
}

void
TodoApp::OnForeground(void)
{
}

void
TodoApp::OnBackground(void)
{
}

void
TodoApp::OnLowMemory(void)
{
}

void
TodoApp::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
}

void
TodoApp::OnScreenOn(void)
{
}

void
TodoApp::OnScreenOff(void)
{
}
