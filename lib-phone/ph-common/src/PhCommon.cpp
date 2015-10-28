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

#include <contacts.h>

#include "ContactsDebug.h"

#include "PhCommon.h"
#include "ContactsCommon.h"

#define UG_CALL_SETTING "setting-call-efl"

PhCommon::PhCommon()
{
}

PhCommon::~PhCommon()
{
}

Evas_Object *PhCommon::createImage(Evas_Object *parent, const char *edj, const char *imagePath)
{
	Evas_Object *icon = elm_image_add(parent);
	WPRET_VM( NULL == icon, NULL, "elm_image_add() is failed");

	if(edj && *edj) {
		elm_image_file_set(icon, ContactsCommon::getResourcePath(edj).c_str(), imagePath);
	} else {
		elm_image_file_set(icon, imagePath, NULL);
	}

	evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	return icon;
}

Evas_Object *PhCommon::createLayout(Evas_Object *parent, const char *edj, const char *layoutName)
{
	Evas_Object *layout = elm_layout_add(parent);
	WPRET_VM(NULL == layout, NULL, "elm_layout_add() return NULL");

	if(edj && *edj) {
		elm_layout_file_set(layout, ContactsCommon::getResourcePath(edj).c_str(), layoutName);

	} else {
		if(layoutName && *layoutName) {
			elm_layout_theme_set(layout, "layout", layoutName, "default");
		} else {
			elm_layout_theme_set( layout, "layout", "application", "default");
		}
	}

	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);

	return layout;
}

Evas_Object *PhCommon::createButton(Evas_Object *parent, const char *buttonStyle, const char *buttonText, Evas_Smart_Cb clickedCb, void *userData)
{
	Evas_Object *button = elm_button_add(parent);
	WPRET_VM( NULL == button, NULL, "elm_button_add() is failed");
	if(buttonStyle) {
		elm_object_style_set(button, buttonStyle);
	}
	if(buttonText) {
		elm_object_text_set(button, buttonText);
	}

	evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_smart_callback_add(button, "clicked", clickedCb, userData);
	evas_object_show(button);

	return button;
}

void PhCommon::launchCallSetting()
{
	WHIT();
	app_control_h service;
	app_control_create(&service);
	app_control_set_app_id(service, UG_CALL_SETTING);
	app_control_set_launch_mode(service, APP_CONTROL_LAUNCH_MODE_GROUP);
	int err = app_control_send_launch_request(service, NULL, NULL);
	WPWARN(err != APP_CONTROL_ERROR_NONE, "app_control_send_launch_request() failed(%d)", err);
	app_control_destroy(service);
}

bool PhCommon::addSpeedDialNumber(int speedNumber, int numberId)
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_speeddial._uri, &filter);
	contacts_filter_add_int(filter, _contacts_speeddial.number_id, CONTACTS_MATCH_EQUAL, numberId);

	contacts_query_h query = nullptr;
	contacts_query_create(_contacts_speeddial._uri, &query);
	contacts_query_set_filter(query, filter);

	contacts_list_h list = nullptr;
	int err = contacts_db_get_records_with_query(query, 0, 1, &list);
	contacts_query_destroy(query);
	contacts_filter_destroy(filter);
	WPRET_VM(err != CONTACTS_ERROR_NONE, false, "contacts_db_get_records_with_query() failed(%d)", err);

	int count = 0;
	contacts_list_get_count(list, &count);
	contacts_list_destroy(list, true);
	if (count > 0) {
		return false;
	}

	contacts_record_h record = nullptr;
	contacts_record_create(_contacts_speeddial._uri, &record);
	contacts_record_set_int(record, _contacts_speeddial.speeddial_number, speedNumber);
	contacts_record_set_int(record, _contacts_speeddial.number_id, numberId);
	err = contacts_db_insert_record(record, nullptr);
	contacts_record_destroy(record, true);
	WPRET_VM(err != CONTACTS_ERROR_NONE, false, "contacts_db_insert_record() failed(%d)", err);

	return true;
}

std::string PhCommon::convert2MarkupString(const char *source)
{
	WPRET_VM(source == NULL, "", "source is NULL");
	std::string markupStr;
	char *temp = elm_entry_utf8_to_markup(source);

	if(temp) {
		markupStr = temp;
		free(temp);
		return markupStr;
	} else {
		return "";
	}
}
