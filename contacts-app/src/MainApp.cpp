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

#include <contacts.h>

#include "MainApp.h"
#include "App/Path.h"
#include "Ui/Naviframe.h"
#include "Ui/Scale.h"
#include "Ui/Window.h"
#include "Utils/Logger.h"

#include "OperationDefaultController.h"
#include "OperationEditController.h"
#include "OperationPickController.h"
#include "OperationViewController.h"

MainApp::MainApp()
	: m_Window(nullptr), m_Navigator(nullptr), m_Controller(nullptr)
{
}

bool MainApp::onCreate()
{
	int err = contacts_connect();
	RETVM_IF_ERR(err, false, "contacts_connect() failed.");

	bindtextdomain(TEXT_DOMAIN, App::getLocaleDir().c_str());
	textdomain(TEXT_DOMAIN);

	app_event_handler_h handle = nullptr;
	ui_app_add_event_handler(&handle, APP_EVENT_LANGUAGE_CHANGED,
			&MainApp::onLanguageChanged, this);

	elm_config_focus_autoscroll_mode_set(ELM_FOCUS_AUTOSCROLL_MODE_NONE);
	elm_app_base_scale_set(UI_BASE_SCALE);

	m_Window = new Ui::Window();
	m_Window->create(nullptr);

	m_Navigator = new Ui::Naviframe();
	m_Window->attachView(m_Navigator);

	return true;
}

void MainApp::onAppControl(app_control_h request)
{
	char *operationStr = nullptr;
	app_control_get_operation(request, &operationStr);

	Operation operation = OperationController::getOperation(operationStr);
	if (!m_Controller || !m_Controller->isOperationSupported(operation)) {
		delete m_Controller;
		m_Controller = nullptr;

		switch (operation) {
			case OperationDefault:
			case OperationDial:
				m_Controller = new OperationDefaultController();
				break;
			case OperationAdd:
			case OperationEdit:
				m_Controller = new OperationEditController();
				break;
			case OperationView:
				m_Controller = new OperationViewController();
				break;
			case OperationPick:
				m_Controller = new OperationPickController();
				break;
		}

		if (m_Controller) {
			m_Controller->create(m_Window, m_Navigator);
		}
	}

	if (m_Controller) {
		m_Controller->request(operation, request);
	}

	free(operationStr);
}

void MainApp::onPause()
{
	if (m_Controller) {
		m_Controller->onPause();
	}
}

void MainApp::onResume()
{
	if (m_Controller) {
		m_Controller->onResume();
	}
}

void MainApp::onTerminate()
{
	delete m_Controller;
	m_Controller = nullptr;

	contacts_disconnect();
}

void MainApp::onLanguageChanged(app_event_info_h event, void *data)
{
	char *lang = nullptr;
	app_event_get_language(event, &lang);
	if (lang) {
		elm_language_set(lang);
		free(lang);
	}
}
