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

#include "OperationDefaultController.h"

#include "MainApp.h"
#include "Contacts/List/MainView.h"
#include "Phone/Dialer/MainView.h"
#include "Ui/TabView.h"

#include <app_preference.h>
#include <badge.h>

#define APP_CONTROL_PHONE_APPID "org.tizen.phone"
#define APP_CONTROL_URI_DIAL "tel:"

#define PREFERENCE_KEY_LAST_TAB "last_tab"

OperationDefaultController::OperationDefaultController()
	: OperationController(OPERATION_DEFAULT | OPERATION_DIAL),
	  m_Navigator(nullptr), m_Tabs{nullptr}
{
}

OperationDefaultController::~OperationDefaultController()
{
	saveLastTab();
}

void OperationDefaultController::onCreate()
{
	Ui::Navigator *mainNavigator = getApplication()->getNavigator();
	m_Navigator = new Ui::TabView();
	mainNavigator->navigateTo(m_Navigator);

	m_Tabs[TabDialer] = new Phone::Dialer::MainView();
	/* TODO:
	m_Tabs[TabLogs] = new Logs::List::LogView();
	 */
	m_Tabs[TabContacts] = new Contacts::List::MainView();

	for (auto &&tab : m_Tabs) {
		m_Navigator->navigateTo(tab);
	}
}

void OperationDefaultController::onRequest(Operation operation, app_control_h request)
{
	char *appId = nullptr;
	app_control_get_app_id(request, &appId);

	TabId selectedTab = TabContacts;
	if (operation == OPERATION_DIAL) {
		auto dialer = static_cast<Phone::Dialer::MainView *>(m_Tabs[TabDialer]);
		dialer->setNumber(getPhoneNumber(request));
		selectedTab = TabDialer;
	} else if (appId && strcmp(appId, APP_CONTROL_PHONE_APPID) == 0) {
		if (getBadgeCount(APP_CONTROL_PHONE_APPID) > 0) {
			selectedTab = TabLogs;
		} else {
			selectedTab = getLastTab();
		}
	}

	m_Navigator->navigateTo(m_Tabs[selectedTab]);

	free(appId);
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

void OperationDefaultController::saveLastTab()
{
	Ui::View *currentTab = m_Navigator->getCurrentView();
	for (int tab = TabFirst; tab < TabMax; ++tab) {
		if (currentTab == m_Tabs[tab]) {
			if (tab != TabContacts) {
				preference_set_int(PREFERENCE_KEY_LAST_TAB, tab);
			}

			break;
		}
	}
}

OperationDefaultController::TabId OperationDefaultController::getLastTab()
{
	TabId lastTab = TabDialer;
	preference_get_int(PREFERENCE_KEY_LAST_TAB, (int *) &lastTab);
	return lastTab;
}
