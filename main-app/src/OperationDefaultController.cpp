/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "OperationDefaultController.h"

#include "WApp.h"
#include "WNaviframe.h"

#include "CtListView.h"
#include "CtTabView.h"
#include "PhDialerView.h"
#include "PhLogMainListView.h"

#include <app_preference.h>
#include <badge.h>

#define APP_CONTROL_PHONE_APPID "org.tizen.phone"
#define APP_CONTROL_URI_DIAL "tel:"

OperationDefaultController::OperationDefaultController()
	: OperationController(OPERATION_DEFAULT | OPERATION_DIAL),
	  m_TabView(nullptr), m_DialerView(nullptr)
{
}

void OperationDefaultController::onCreate()
{
	WNaviframe *naviframe = static_cast<WNaviframe*>(getApplication()->getWindow()->getBaseUiObject());
	m_TabView = createTabView();
	naviframe->push(m_TabView);
}

void OperationDefaultController::onAppControl(Operation operation, app_control_h request)
{
	char *appId = nullptr;
	app_control_get_app_id(request, &appId);

	CtTabSubView selectedView = TAB_VIEW_CONTACTS;
	if (operation == OPERATION_DIAL) {
		selectedView = TAB_VIEW_DIALER;
		m_DialerView->setNumber(getPhoneNumber(request));
	} else if (appId && strcmp(appId, APP_CONTROL_PHONE_APPID) == 0) {
		if (getBadgeCount(APP_CONTROL_PHONE_APPID) > 0) {
			selectedView = TAB_VIEW_LOGS;
		} else {
			selectedView = (CtTabSubView) getLastTab();
		}
	}

	m_TabView->reset(selectedView);

	free(appId);
}

CtTabView *OperationDefaultController::createTabView()
{
	CtTabView *tabView = new CtTabView(TAB_VIEW_CONTACTS, false);
	m_DialerView = new PhDialerView();
	tabView->insert(TAB_VIEW_DIALER, m_DialerView);
	tabView->insert(TAB_VIEW_LOGS, new PhLogMainListView());
	tabView->insert(TAB_VIEW_CONTACTS, new CtListView(LIST_VIEW_DEFAULT, LIST_DATA_DEFAULT, LIST_CTRL_DEFAULT));

	return tabView;
}

std::string OperationDefaultController::getPhoneNumber(app_control_h appControl)
{
	std::string number;

	char *uri = nullptr;
	app_control_get_uri(appControl, &uri);

	size_t length = sizeof(APP_CONTROL_URI_DIAL) - 1;
	if (uri && strncmp(uri, APP_CONTROL_URI_DIAL, length) == 0) {
		number = uri + length;
	}

	free(uri);
	return number;
}

unsigned OperationDefaultController::getBadgeCount(const char *package)
{
	unsigned count = 0;
	badge_get_count(package, &count);
	return count;
}

int OperationDefaultController::getLastTab()
{
	int value = TAB_VIEW_DIALER;
	preference_get_int(CT_KEY_LAST_SELECTED_TAB_INDEX_FOR_PHONE, &value);
	return value;
}
