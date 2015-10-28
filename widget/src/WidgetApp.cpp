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

#include "WidgetApp.h"
#include "Widget.h"

#include "ContactsCommon.h"
#include "ContactsDebug.h"
#include <contacts.h>

bool WidgetApp::onCreate()
{
	ContactsCommon::loadLocalization();
	int err = contacts_connect();
	WPRET_VM(err != CONTACTS_ERROR_NONE, false, "contacts_connect() failed(%d)", err);

	app_event_handler_h handle = nullptr;
	widget_app_add_event_handler(&handle, APP_EVENT_LANGUAGE_CHANGED,
			&WidgetApp::onLanguageChanged, this);

	return true;
}

void WidgetApp::onTerminate()
{
	contacts_disconnect();
}

WWidget *WidgetApp::onWidgetCreate()
{
	return new Widget();
}

void WidgetApp::onLanguageChanged(app_event_info_h event, void *data)
{
	char *lang = nullptr;
	app_event_get_language(event, &lang);
	if (lang) {
		elm_language_set(lang);
		free(lang);
	}
}
