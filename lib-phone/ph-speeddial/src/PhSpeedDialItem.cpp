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

#include "PhSpeedDialItem.h"
#include "PhSpeedDialLayout.h"
#include "PhCommon.h"

#include "ContactsAppControl.h"
#include "ContactsCommon.h"
#include "ContactsDebug.h"
#include "ContactsLocalization.h"
#include "ContactsThumbnail.h"

#include <notification.h>

#define TEXT_BUFFER_SIZE 256

namespace
{
	const std::string layoutPath = ContactsCommon::getResourcePath(SPEED_DIAL_LAYOUT_EDJ);
}

PhSpeedDialItem::PhSpeedDialItem(int number)
	: m_Number(number), m_Chooser(nullptr)
{
}

PhSpeedDialItem::~PhSpeedDialItem()
{
	closeChooser();
}

void PhSpeedDialItem::setData(contacts_record_h record)
{
	if (record) {
		setLayoutData(getEvasObj(), record);
	} else {
		setEmptyLayout(getEvasObj());
	}
}

Evas_Object *PhSpeedDialItem::onCreate(Evas_Object *parent, void *param)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, layoutPath.c_str(), GROUP_ITEM_LAYOUT);
	return layout;
}

Evas_Object *PhSpeedDialItem::createAddButton(Evas_Object *parent)
{
	Evas_Object *button = elm_image_add(parent);
	elm_image_file_set(button, layoutPath.c_str(), GROUP_ITEM_ICON);
	evas_object_smart_callback_add(button, "clicked",
			&PhSpeedDialItem::onItemPressed, this);

	return button;
}

Evas_Object *PhSpeedDialItem::createDeleteButton(Evas_Object *parent)
{
	Evas_Object *button = elm_image_add(parent);
	elm_image_file_set(button, layoutPath.c_str(), GROUP_DELETE_BUTTON);
	evas_object_smart_callback_add(button, "clicked",
			&PhSpeedDialItem::onDeletePressed, this);

	return button;
}

void PhSpeedDialItem::setEmptyLayout(Evas_Object *layout)
{
	elm_object_part_content_set(layout, PART_ICON, createAddButton(layout));
	elm_object_part_content_set(layout, PART_DELETE_BUTTON, nullptr);

	char buffer[TEXT_BUFFER_SIZE];
	snprintf(buffer, sizeof(buffer), "%d", m_Number);
	elm_object_part_text_set(layout, PART_NAME, buffer);
}

void PhSpeedDialItem::setLayoutData(Evas_Object *layout, contacts_record_h record)
{
	char *name = nullptr;
	char *imagePath = nullptr;

	contacts_record_get_str_p(record, _contacts_speeddial.display_name, &name);
	contacts_record_get_str_p(record, _contacts_speeddial.image_thumbnail_path, &imagePath);

	elm_object_part_content_set(layout, PART_ICON, createThumbnail(layout, THUMBNAIL_240, imagePath));
	elm_object_part_content_set(layout, PART_DELETE_BUTTON, createDeleteButton(layout));

	char buffer[TEXT_BUFFER_SIZE];
	snprintf(buffer, sizeof(buffer), "%d. %s", m_Number, name);
	elm_object_part_text_set(layout, PART_NAME, buffer);
}

void PhSpeedDialItem::fetchData()
{
	contacts_record_h record = nullptr;
	contacts_db_get_record(_contacts_speeddial._uri, m_Number, &record);

	setData(record);
	contacts_record_destroy(record, true);
}

void PhSpeedDialItem::closeChooser()
{
	if (m_Chooser) {
		app_control_send_terminate_request(m_Chooser);
		app_control_destroy(m_Chooser);
		m_Chooser = nullptr;
	}
}

void PhSpeedDialItem::onPickResult(app_control_h request, app_control_h reply,
		app_control_result_e result, void *data)
{
	PhSpeedDialItem *item = (PhSpeedDialItem*) data;

	char **numberIds = nullptr;
	int count = 0;
	int err = app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &numberIds, &count);
	WPRET_M(err != APP_CONTROL_ERROR_NONE, "app_control_get_extra_data() failed(%d)", err);

	if (numberIds && numberIds[0]) {
		int numberId = atoi(numberIds[0]);

		if (PhCommon::addSpeedDialNumber(item->m_Number, numberId)) {
			item->fetchData();
		} else {
			notification_status_message_post(T_("IDS_PB_POP_ALREADY_EXISTS_LC"));
		}
	}

	for (int i = 0; i < count; ++i) {
		free(numberIds[i]);
	}
	free(numberIds);
}

void PhSpeedDialItem::onItemPressed(void *data, Evas_Object *obj, void *event_info)
{
	PhSpeedDialItem *item = (PhSpeedDialItem*) data;
	item->closeChooser();

	int err = launchContactPick(APP_CONTROL_DATA_SELECTION_MODE_SINGLE, APP_CONTROL_DATA_TYPE_PHONE,
			&PhSpeedDialItem::onPickResult, data, true, &item->m_Chooser);
	WPWARN(err != APP_CONTROL_ERROR_NONE, "launchContactPick() failed(%d)", err);
}

void PhSpeedDialItem::onDeletePressed(void *data, Evas_Object *obj, void *event_info)
{
	PhSpeedDialItem *item = (PhSpeedDialItem*) data;

	int err = contacts_db_delete_record(_contacts_speeddial._uri, item->m_Number);
	WPRET_M(err != CONTACTS_ERROR_NONE, "contacts_db_delete_record() failed(%d)", err);
	item->setEmptyLayout(item->getEvasObj());

	char buffer[TEXT_BUFFER_SIZE];
	snprintf(buffer, sizeof(buffer), T_("IDS_KPD_TPOP_SPEED_DIAL_NUMBER_PD_REMOVED"), item->m_Number);
	notification_status_message_post(buffer);
}
