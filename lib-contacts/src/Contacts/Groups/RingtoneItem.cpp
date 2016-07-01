/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Contacts/Groups/RingtoneItem.h"

#include "App/AppControlRequest.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include <app_i18n.h>
#include <system_settings.h>

using namespace Contacts::Groups;

const std::string &RingtoneItem::getPath() const
{
	return m_Path;
}

Elm_Genlist_Item_Class *RingtoneItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass("type2");
	return &itc;
}

char *RingtoneItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		char *value = getRingtonePath();
		char *substr = strdup(basename(value));
		free(value);
		return substr;
	} else if (strcmp(part, "elm.text.sub") == 0) {
		return strdup(_("IDS_PB_OPT_RINGTONE"));
	}
	return nullptr;
}

void RingtoneItem::onSelected()
{
	pickRingtone();
}

char *RingtoneItem::getRingtonePath() const
{
	char *value = nullptr;
	if (!m_Path.empty()) {
		value = strdup(m_Path.c_str());
	} else {
		char *defaultValue = nullptr;
		system_settings_get_value_string(SYSTEM_SETTINGS_KEY_INCOMING_CALL_RINGTONE, &defaultValue);
		value = defaultValue;
	}

	return value;
}

void RingtoneItem::pickRingtone()
{
	char *value = getRingtonePath();
	m_AppControl = App::requestPickRingtone(value);
	m_AppControl.launch(makeCallbackWithLastParam(&RingtoneItem::onPickResult), this);
	free(value);
}

void RingtoneItem::onPickResult(app_control_h request, app_control_h reply,
		app_control_result_e result)
{
	char *path = nullptr;
	int err = app_control_get_extra_data(reply, "result", &path);
	WARN_IF_ERR(err, "app_control_get_extra_data() failed.");
	m_Path = path ? path : "";
	free(path);
}
