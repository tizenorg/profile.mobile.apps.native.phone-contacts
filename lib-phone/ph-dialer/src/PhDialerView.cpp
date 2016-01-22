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

#include "PhDialerView.h"
#include "PhDialerEntry.h"
#include "PhDialerKey.h"
#include "PhDialerLayout.h"
#include "PhDialerSpeeddialPopup.h"
#include "PhSpeedDialView.h"
#include "PhDialerSearchController.h"

#include "CommonPath.h"
#include "ContactsAppControl.h"
#include "ContactsDebug.h"
#include "ContactsLocalization.h"
#include "PhCommon.h"
#include "WMenuPopup.h"

#include <app_control.h>
#include <contacts.h>
#include <feedback.h>

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 3

namespace
{
	const std::string layoutFilePath = ContactsCommon::getResourcePath(DIALER_LAYOUT_EDJ);
}

PhDialerView::PhDialerView()
	: m_Entry(nullptr), m_Controller(nullptr)
{
	feedback_initialize();
}

PhDialerView::~PhDialerView()
{
	feedback_deinitialize();
}

void PhDialerView::setPredictiveLayout(Evas_Object *layout)
{
	elm_object_part_content_set(getEvasObj(), PART_SWALLOW_PREDICTIVE, layout);
}

PhDialerEntry* PhDialerView::getEntry() const
{
	return m_Entry;
}

Evas_Object *PhDialerView::onCreate(Evas_Object *parent, void *viewParam)
{
	elm_theme_extension_add(nullptr, ContactsCommon::getResourcePath(COMMON_BUTTONS_EDJ).c_str());

	Evas_Object *layout = elm_layout_add(parent);
	Eina_Bool res = elm_layout_file_set(layout, layoutFilePath.c_str(), GROUP_DIALER);
	WPWARN(res != EINA_TRUE, "elm_layout_file_set() failed");

	m_Entry = new PhDialerEntry();
	m_Entry->create(layout, nullptr);
	elm_object_part_content_set(layout, PART_SWALLOW_ENTRY, m_Entry->getEvasObj());
	elm_object_part_content_set(layout, PART_SWALLOW_KEYPAD, createKeypad(layout));
	elm_object_part_content_set(layout, PART_SWALLOW_CALL, createCallButton(layout));
	elm_object_part_content_set(layout, PART_SWALLOW_BACKSPACE, createBackspaceButton(layout));

	m_Controller = new PhDialerSearchController();
	m_Controller->create(*this);

	m_Entry->setNumber(m_InitialNumber);

	return layout;
}

void PhDialerView::setNumber(const std::string &number)
{
	m_InitialNumber = number;
	if (m_Entry) {
		m_Entry->setNumber(number);
	}
}

void PhDialerView::onPush(Elm_Object_Item *naviItem)
{
	enableMoreButton(naviItem, &PhDialerView::onMenuPressed, this);
}

void PhDialerView::onTabSelect(Elm_Object_Item *naviItem)
{
	Evas_Object *conf = getWindow()->getConformantEvasObj();
	elm_object_signal_emit(conf, "elm,state,virtualkeypad,disable", "");
	elm_object_signal_emit(conf, "elm,state,clipboard,disable", "");

	enableMoreButton(naviItem, &PhDialerView::onMenuPressed, this);
}

void PhDialerView::onTabUnselect(Elm_Object_Item *naviItem)
{
	Evas_Object *conf = getWindow()->getConformantEvasObj();
	elm_object_signal_emit(conf, "elm,state,virtualkeypad,enable", "");
	elm_object_signal_emit(conf, "elm,state,clipboard,enable", "");
}

bool PhDialerView::onTabPop()
{
	return true;
}

void PhDialerView::onTabScrollStart()
{
}

void PhDialerView::onTabScrollStop()
{
}

Evas_Object *PhDialerView::createKeypad(Evas_Object *parent)
{
	Evas_Object *table = elm_table_add(parent);
	elm_table_padding_set(table, 2, 2);

	int id = 0;
	for(int i = 0; i < KEYPAD_ROWS; ++i) {
		for(int j = 0; j < KEYPAD_COLS; ++j, ++id) {
			using namespace std::placeholders;

			PhDialerKey *key = new PhDialerKey((PhDialerKey::Id) id);
			key->create(table, nullptr);
			key->setOnPressed(std::bind(&PhDialerView::onKeyPressed, this, _1));
			key->setOnLongpressed(std::bind(&PhDialerView::onKeyLongpressed, this, _1));

			Evas_Object *button = key->getEvasObj();
			evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
			elm_table_pack(table, button, j, i, 1, 1);
			evas_object_show(button);
		}
	}

	return table;
}

Evas_Object *PhDialerView::createCallButton(Evas_Object *parent)
{
	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, "custom_circle");
	evas_object_smart_callback_add(button, "clicked",
			&PhDialerView::onCallPressed, this);

	Evas_Object *edje = elm_layout_edje_get(button);
	edje_object_color_class_set(edje, "button_normal", BUTTON_CALL_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0);
	edje_object_color_class_set(edje, "button_pressed", BUTTON_CALL_PRESSED, 0, 0, 0, 0, 0, 0, 0, 0);

	Evas_Object *image = elm_image_add(button);
	Eina_Bool res = elm_image_file_set(image, layoutFilePath.c_str(), GROUP_BUTTON_CALL);
	WPWARN(res != EINA_TRUE, "elm_layout_file_set() failed");
	elm_object_part_content_set(button, "elm.swallow.content", image);

	return button;
}

Evas_Object *PhDialerView::createBackspaceButton(Evas_Object *parent)
{
	using namespace std::placeholders;
	WButton *key = new WButton();
	key->create(parent, nullptr);
	key->setOnPressed(std::bind(&PhDialerView::onBackspacePressed, this, _1));
	key->setOnLongpressed(std::bind(&PhDialerView::onBackspaceLongpressed, this, _1));

	Evas_Object *button = key->getEvasObj();
	elm_object_style_set(button, "transparent");
	Evas_Object *image = elm_image_add(button);
	Eina_Bool res = elm_image_file_set(image, layoutFilePath.c_str(), GROUP_BUTTON_BACKSPACE);
	WPWARN(res != EINA_TRUE, "elm_layout_file_set() failed");
	elm_object_part_content_set(button, "elm.swallow.content", image);

	return button;
}

void PhDialerView::onKeyPressed(WButton &button)
{
	PhDialerKey &key = static_cast<PhDialerKey&>(button);
	m_Entry->insert(key.getValue());
}

bool PhDialerView::onKeyLongpressed(WButton &button)
{
	PhDialerKey &key = static_cast<PhDialerKey&>(button);
	int id = key.getId();

	if (m_Entry->getNumber().empty()) {
		if (id >= PhDialerKey::ID_1 && id <= PhDialerKey::ID_9) {
			launchSpeeddial(key.getValue() - '0');
			return true;
		}
	}

	if (id == PhDialerKey::ID_0) {
		m_Entry->insert('+');
		return true;
	}

	return false;
}

void PhDialerView::onBackspacePressed(WButton &button)
{
	feedback_play(FEEDBACK_PATTERN_KEY_BACK);
	m_Entry->popBack();
}

bool PhDialerView::onBackspaceLongpressed(WButton &button)
{
	m_Entry->clear();
	return true;
}

void PhDialerView::onCallPressed(void *data, Evas_Object *obj, void *event_info)
{
	PhDialerView *view = (PhDialerView*) data;

	std::string number = view->m_Entry->getNumber();
	if (!number.empty()) {
		launchCall(number);
		view->m_Entry->clear();
	} else {
		view->m_Entry->setNumber(getLastNumber());
	}
}

void PhDialerView::onMenuPressed(void *data, Evas_Object *obj, void *event_info)
{
	PhDialerView *view = (PhDialerView*) data;

	WMenuPopup *popup = new WMenuPopup();
	popup->prepare(view->getWindow()->getEvasObj(),
			view->getNaviframe()->getEvasObj());

	if (!view->m_Entry->getNumber().empty()) {
		popup->appendItem(T_("IDS_KPD_BUTTON_SEND_MESSAGE"),
				std::bind(&PhDialerView::onSendMessage, view));
		popup->appendItem(T_("IDS_KPD_OPT_ADD_2_SECOND_PAUSE_ABB"),
				std::bind(&PhDialerView::onAddPause, view));
		popup->appendItem(T_("IDS_KPD_OPT_ADD_WAIT_ABB"),
				std::bind(&PhDialerView::onAddWait, view));
	}

	popup->appendItem(T_("IDS_KPD_OPT_SPEED_DIAL_SETTINGS_ABB2"),
			std::bind(&PhDialerView::onSpeeddialSettings, view));
	popup->appendItem(T_("IDS_KPD_OPT_CALL_SETTINGS_ABB"),
			std::bind(&PhDialerView::onCallSettings, view));

	view->attachPopup(popup);
}

void PhDialerView::onSendMessage()
{
	int err = launchMessageComposer("sms:", m_Entry->getNumber().c_str());
	WPWARN(err != APP_CONTROL_ERROR_NONE, "launchMessageComposer() failed(0x%x)", err);
}

void PhDialerView::onAddWait()
{
	m_Entry->insert(';');
}

void PhDialerView::onAddPause()
{
	m_Entry->insert(',');
}

void PhDialerView::onSpeeddialSettings()
{
	getNaviframe()->push(new PhSpeedDialView(), NULL, NULL);
}

void PhDialerView::onCallSettings()
{
	PhCommon::launchCallSetting();
}

void PhDialerView::launchCall(const std::string &number)
{
	int err = ::launchCall(number.c_str());
	WPWARN(err != APP_CONTROL_ERROR_NONE, "launchCall() failed(0x%x)", err);
}

void PhDialerView::launchSpeeddial(int digit)
{
	std::string number = getSpeeddialNumber(digit);
	if (!number.empty()) {
		launchCall(number);
	} else {
		attachPopup(new PhDialerSpeeddialPopup(digit));
	}
}

std::string PhDialerView::getSpeeddialNumber(int digit)
{
	std::string number;
	contacts_filter_h filter = NULL;
	contacts_query_h query = NULL;
	contacts_list_h list = NULL;

	contacts_filter_create(_contacts_speeddial._uri, &filter);
	contacts_filter_add_int(filter, _contacts_speeddial.speeddial_number, CONTACTS_MATCH_EQUAL, digit);

	contacts_query_create(_contacts_speeddial._uri, &query);
	contacts_query_set_filter(query, filter);

	int err = contacts_db_get_records_with_query(query, 0, 1, &list);
	WPWARN(err != CONTACTS_ERROR_NONE, "contacts_db_get_records_with_query() failed(0x%x)", err);
	if (list) {
		contacts_record_h record = NULL;
		contacts_list_get_current_record_p(list, &record);
		if (record) {
			char *str = NULL;
			contacts_record_get_str_p(record, _contacts_speeddial.number, &str);
			if (str) {
				number = str;
			}
		}

		contacts_list_destroy(list, true);
	}

	contacts_query_destroy(query);
	contacts_filter_destroy(filter);

	return number;
}

std::string PhDialerView::getLastNumber()
{
	std::string number;
	contacts_list_h list = NULL;
	contacts_query_h query = NULL;
	contacts_filter_h filter = NULL;

	contacts_filter_create(_contacts_person_phone_log._uri, &filter);
	contacts_filter_add_int(filter, _contacts_person_phone_log.log_type,
			CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMING);
	contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	contacts_filter_add_int(filter, _contacts_person_phone_log.log_type,
			CONTACTS_MATCH_LESS_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VIDEO_BLOCKED);

	contacts_query_create(_contacts_person_phone_log._uri, &query);
	contacts_query_set_filter(query, filter);
	contacts_query_set_sort(query, _contacts_person_phone_log.log_time, false);

	int err = contacts_db_get_records_with_query(query, 0, 1, &list);
	WPWARN(err != CONTACTS_ERROR_NONE, "contacts_db_get_records_with_query() failed(0x%x)", err);
	if (list) {
		contacts_record_h record = NULL;
		contacts_list_get_current_record_p(list, &record);
		if (record) {
			char *str = NULL;
			contacts_record_get_str_p(record, _contacts_person_phone_log.address, &str);
			if (str) {
				number = str;
			}
		}

		contacts_list_destroy(list, true);
	}

	contacts_query_destroy(query);
	contacts_filter_destroy(filter);

	return number;
}
