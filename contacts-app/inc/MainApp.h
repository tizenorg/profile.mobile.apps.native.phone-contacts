/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef MAIN_APP_H
#define MAIN_APP_H

#include "App/Application.h"

namespace Ui
{
	class Navigator;
	class Window;
}

class OperationController;

class MainApp : public App::Application
{
public:
	MainApp();

private:
	virtual bool onCreate() override;
	virtual void onAppControl(app_control_h request) override;
	virtual void onPause() override;
	virtual void onResume() override;
	virtual void onTerminate() override;
	static void onLanguageChanged(app_event_info_h event, void *data);

	Ui::Window *m_Window;
	Ui::Navigator *m_Navigator;
	OperationController *m_Controller;
};

#endif /* MAIN_APP_H */
