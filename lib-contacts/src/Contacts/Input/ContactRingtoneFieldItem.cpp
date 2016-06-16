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

#include "Contacts/Input/ContactRingtoneFieldItem.h"
#include "Contacts/Model/ContactObject.h"
#include "Contacts/Model/ContactTextField.h"
#include "Contacts/Common/Strings.h"

#include "App/AppControlRequest.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include "InputItemLayout.h"

#include <app_i18n.h>
#include <system_settings.h>

using namespace Contacts::Input;
using namespace Contacts::Model;

bool ContactRingtoneFieldItem::isFocusable() const
{
	return false;
}

Elm_Genlist_Item_Class *ContactRingtoneFieldItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass("type2");
	return &itc;
}

char *ContactRingtoneFieldItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		char *value = getRingtonePath();
		char *substr = strdup(basename(value));
		free(value);
		return substr;
	} else if (strcmp(part, "elm.text.sub") == 0) {
		const char *name = Common::getContactFieldName(ContactFieldId(getObject().getId()));
		return Utils::safeDup(_(name));
	}

	return nullptr;
}

Evas_Object *ContactRingtoneFieldItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.swallow.end") == 0) {
		return ContactFieldItem::getContent(parent, PART_BUTTON);
	}

	return nullptr;
}

void ContactRingtoneFieldItem::onInserted()
{
	ContactFieldItem::onInserted();
	elm_genlist_item_select_mode_set(getObjectItem(), ELM_OBJECT_SELECT_MODE_DEFAULT);
}

void ContactRingtoneFieldItem::onSelected()
{
	pickRingtone();
}

void ContactRingtoneFieldItem::onFocused()
{
	pickRingtone();
}

char *ContactRingtoneFieldItem::getRingtonePath() const
{
	char *value = nullptr;
	const char *fieldValue = getField().cast<ContactTextField>().getValue();
	if (fieldValue) {
		value = strdup(fieldValue);
	} else {
		char *defaultValue = nullptr;
		system_settings_get_value_string(SYSTEM_SETTINGS_KEY_INCOMING_CALL_RINGTONE, &defaultValue);
		value = defaultValue;
	}

	return value;
}

void ContactRingtoneFieldItem::pickRingtone()
{
	char *value = getRingtonePath();
	m_AppControl = App::requestPickRingtone(value);
	m_AppControl.launch(makeCallbackWithLastParam(&ContactRingtoneFieldItem::onPickResult), this);
	free(value);
}

void ContactRingtoneFieldItem::onPickResult(app_control_h request, app_control_h reply,
		app_control_result_e result)
{
	char *path = nullptr;
	int err = app_control_get_extra_data(reply, "result", &path);
	WARN_IF_ERR(err, "app_control_get_extra_data() failed.");

	getField().cast<ContactTextField>().setValue(path);
	free(path);
}
