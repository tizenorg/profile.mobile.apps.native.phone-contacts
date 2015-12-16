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

#include "Phone/SpeedDial/SettingsItem.h"
#include "Phone/Utils.h"

#include "App/AppControlRequest.h"
#include "App/Path.h"
#include "Ui/Thumbnail.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include "SpeedDialLayout.h"

#include <notification.h>

#define TEXT_BUFFER_SIZE 256

namespace
{
	const std::string layoutPath = App::getResourcePath(SPEED_DIAL_LAYOUT_EDJ);
}

using namespace Phone;
using namespace Phone::SpeedDial;

SettingsItem::SettingsItem(int number)
	: m_Number(number), m_Chooser(nullptr)
{
}

void SettingsItem::setData(contacts_record_h record)
{
	if (record) {
		setLayoutData(record);
	} else {
		setEmptyLayout();
	}
}

Evas_Object *SettingsItem::onCreate(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, layoutPath.c_str(), GROUP_ITEM_LAYOUT);
	return layout;
}

Evas_Object *SettingsItem::createAddButton(Evas_Object *parent)
{
	Evas_Object *button = elm_image_add(parent);
	elm_image_file_set(button, layoutPath.c_str(), GROUP_ITEM_ICON);
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&SettingsItem::onAddPressed), this);

	return button;
}

Evas_Object *SettingsItem::createDeleteButton(Evas_Object *parent)
{
	Evas_Object *button = elm_image_add(parent);
	elm_image_file_set(button, layoutPath.c_str(), GROUP_DELETE_BUTTON);
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&SettingsItem::onDeletePressed), this);

	return button;
}

void SettingsItem::setEmptyLayout()
{
	Evas_Object *layout = getEvasObject();
	elm_object_part_content_set(layout, PART_ICON, createAddButton(layout));
	elm_object_part_content_set(layout, PART_DELETE_BUTTON, nullptr);

	char buffer[TEXT_BUFFER_SIZE];
	snprintf(buffer, sizeof(buffer), "%d", m_Number);
	elm_object_part_text_set(layout, PART_NAME, buffer);
}

void SettingsItem::setLayoutData(contacts_record_h record)
{
	using Ui::Thumbnail;

	char *name = nullptr;
	char *imagePath = nullptr;

	contacts_record_get_str_p(record, _contacts_speeddial.display_name, &name);
	contacts_record_get_str_p(record, _contacts_speeddial.image_thumbnail_path, &imagePath);

	Evas_Object *layout = getEvasObject();
	Thumbnail *thumbnail = Thumbnail::create(layout, Thumbnail::SizeLarge, imagePath);

	elm_object_part_content_set(layout, PART_ICON, thumbnail->getEvasObject());
	elm_object_part_content_set(layout, PART_DELETE_BUTTON, createDeleteButton(layout));

	char buffer[TEXT_BUFFER_SIZE];
	snprintf(buffer, sizeof(buffer), "%d. %s", m_Number, name);
	elm_object_part_text_set(layout, PART_NAME, buffer);
}

void SettingsItem::onPickResult(app_control_h request, app_control_h reply,
		app_control_result_e result)
{
	char **numberIds = nullptr;
	int count = 0;
	int err = app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &numberIds, &count);
	RETM_IF_ERR(err, "app_control_get_extra_data() failed.");

	if (numberIds && numberIds[0]) {
		int numberId = atoi(numberIds[0]);

		if (!addSpeedDialNumber(m_Number, numberId)) {
			notification_status_message_post(_("IDS_PB_POP_ALREADY_EXISTS_LC"));
		}
	}

	for (int i = 0; i < count; ++i) {
		free(numberIds[i]);
	}
	free(numberIds);
}

void SettingsItem::onAddPressed(Evas_Object *obj, void *event_info)
{
	m_Chooser = App::requestContactPick(APP_CONTROL_SELECT_SINGLE, APP_CONTROL_RESULT_PHONE);
	m_Chooser.launch(makeCallbackWithLastParam(&SettingsItem::onPickResult), this);
}

void SettingsItem::onDeletePressed(Evas_Object *obj, void *event_info)
{
	int err = contacts_db_delete_record(_contacts_speeddial._uri, m_Number);
	RETM_IF_ERR(err, "contacts_db_delete_record() failed.");

	char buffer[TEXT_BUFFER_SIZE];
	snprintf(buffer, sizeof(buffer), _("IDS_KPD_TPOP_SPEED_DIAL_NUMBER_PD_REMOVED"), m_Number);
	notification_status_message_post(buffer);
}
