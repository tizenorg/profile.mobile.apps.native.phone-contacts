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

#include "Phone/Dialer/DialerView.h"
#include "Phone/Dialer/KeypadButton.h"
#include "Phone/Dialer/KeypadEntry.h"
#include "Phone/Dialer/SearchResultsControl.h"
#include "Phone/Dialer/SpeedDialPopup.h"
#include "Phone/SpeedDial/SettingsView.h"
#include "Phone/Utils.h"

#include "App/AppControlRequest.h"
#include "App/Path.h"
#include "Ui/ListPopup.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Ui/Window.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include "AppsCommonButtons.h"
#include "DialerLayout.h"

#include <app_control.h>
#include <contacts.h>
#include <feedback.h>

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 3

namespace
{
	const std::string layoutFilePath = App::getResourcePath(DIALER_LAYOUT_EDJ);
}

using namespace Phone::Dialer;
using namespace std::placeholders;

DialerView::DialerView()
	: m_Entry(nullptr), m_SearchControl(nullptr)
{
}

DialerView::~DialerView()
{
	feedback_deinitialize();
	contacts_db_remove_changed_cb(_contacts_contact._uri,
			makeCallbackWithLastParam(&DialerView::onDbChanged), this);
}

void DialerView::onCreated()
{
	feedback_initialize();
	contacts_db_add_changed_cb(_contacts_speeddial._uri,
			makeCallbackWithLastParam(&DialerView::onDbChanged), this);
	contacts_db_add_changed_cb(_contacts_contact._uri,
			makeCallbackWithLastParam(&DialerView::onDbChanged), this);
	contacts_db_add_changed_cb(_contacts_phone_log._uri,
			makeCallbackWithLastParam(&DialerView::onDbChanged), this);
}

Evas_Object *DialerView::onCreate(Evas_Object *parent)
{
	elm_theme_extension_add(nullptr, App::getResourcePath(APPS_COMMON_BUTTONS_EDJ).c_str());

	Evas_Object *layout = elm_layout_add(parent);
	Eina_Bool res = elm_layout_file_set(layout, layoutFilePath.c_str(), GROUP_DIALER);
	WARN_IF(res != EINA_TRUE, "elm_layout_file_set() failed");

	elm_object_part_content_set(layout, PART_SWALLOW_ENTRY, createEntry(layout));
	elm_object_part_content_set(layout, PART_SWALLOW_PREDICTIVE, createSearchControl(layout));
	elm_object_part_content_set(layout, PART_SWALLOW_KEYPAD, createKeypad(layout));
	elm_object_part_content_set(layout, PART_SWALLOW_CALL, createCallButton(layout));
	elm_object_part_content_set(layout, PART_SWALLOW_BACKSPACE, createBackspaceButton(layout));

	return layout;
}

void DialerView::setNumber(const std::string &number)
{
	if (m_Entry) {
		m_Entry->setNumber(number);
	}
}

void DialerView::onPageAttached(Ui::NavigatorPage *page)
{
	page->setTitle("IDS_KPD_ITAB3_KEYPAD");
}

void DialerView::onNavigation(bool isCurrentView)
{
	Ui::Window *window = findParent<Ui::Window>();
	RETM_IF(!window, "findParent() failed");

	Evas_Object *conf = window->getConformant();
	if (isCurrentView) {
		elm_object_signal_emit(conf, "elm,state,virtualkeypad,disable", "");
		elm_object_signal_emit(conf, "elm,state,clipboard,disable", "");
	} else {
		elm_object_signal_emit(conf, "elm,state,virtualkeypad,enable", "");
		elm_object_signal_emit(conf, "elm,state,clipboard,enable", "");
	}
}

void DialerView::onMenuPressed()
{
	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());

	if (!m_Entry->getNumber().empty()) {
		menu->addItem("IDS_KPD_BUTTON_SEND_MESSAGE", [this] {
			m_AppControl = App::requestMessageComposer("sms:", m_Entry->getNumber().c_str());
			m_AppControl.launch();
		});
		menu->addItem("IDS_KPD_OPT_ADD_2_SECOND_PAUSE_ABB", [this] {
			m_Entry->insert(',');
		});
		menu->addItem("IDS_KPD_OPT_ADD_WAIT_ABB", [this] {
			m_Entry->insert(';');
		});
	}

	menu->addItem("IDS_KPD_OPT_SPEED_DIAL_SETTINGS_ABB2", [this] {
		getNavigator()->navigateTo(new SpeedDial::SettingsView());
	});
	menu->addItem("IDS_KPD_OPT_CALL_SETTINGS_ABB", [this] {
		m_AppControl = App::requestCallSettings();
		m_AppControl.launch();
	});

	menu->show();
}

Evas_Object *DialerView::createEntry(Evas_Object *parent)
{
	m_Entry = new KeypadEntry();
	m_Entry->setChangedCallback(std::bind(&DialerView::onEntryChanged, this));
	return m_Entry->create(parent);
}

Evas_Object *DialerView::createSearchControl(Evas_Object *parent)
{
	m_SearchControl = new SearchResultsControl();
	m_SearchControl->setSelectedCallback(std::bind(&DialerView::onResultSelected, this, _1));
	return m_SearchControl->create(parent);
}

Evas_Object *DialerView::createKeypad(Evas_Object *parent)
{
	Evas_Object *table = elm_table_add(parent);
	elm_table_padding_set(table, 2, 2);

	int id = KeypadButton::ID_1;
	for (int i = 0; i < KEYPAD_ROWS; ++i) {
		for (int j = 0; j < KEYPAD_COLS; ++j, ++id) {
			KeypadButton *key = new KeypadButton((KeypadButton::Id) id);
			key->setPressedCallback(std::bind(&DialerView::onKeyPressed, this, _1));
			key->setLongpressedCallback(std::bind(&DialerView::onKeyLongpressed, this, _1));

			Evas_Object *button = key->create(table);
			evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
			elm_table_pack(table, button, j, i, 1, 1);
			evas_object_show(button);
		}
	}

	return table;
}

Evas_Object *DialerView::createCallButton(Evas_Object *parent)
{
	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, BUTTON_STYLE_CUSTOM_CIRCLE);
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&DialerView::onCallPressed), this);

	Evas_Object *edje = elm_layout_edje_get(button);
	edje_object_color_class_set(edje, BUTTON_COLOR_CLASS_NORMAL, BUTTON_CALL_NORMAL,
			0, 0, 0, 0, 0, 0, 0, 0);
	edje_object_color_class_set(edje, BUTTON_COLOR_CLASS_PRESSED, BUTTON_CALL_PRESSED,
			0, 0, 0, 0, 0, 0, 0, 0);

	Evas_Object *image = elm_image_add(button);
	Eina_Bool res = elm_image_file_set(image, layoutFilePath.c_str(), GROUP_BUTTON_CALL);
	WARN_IF(res != EINA_TRUE, "elm_layout_file_set() failed");
	elm_object_part_content_set(button, "elm.swallow.content", image);

	return button;
}

Evas_Object *DialerView::createBackspaceButton(Evas_Object *parent)
{
	Ui::Button *key = new Ui::Button();
	key->setPressedCallback(std::bind(&DialerView::onBackspacePressed, this, _1));
	key->setLongpressedCallback(std::bind(&DialerView::onBackspaceLongpressed, this, _1));

	Evas_Object *button = key->create(parent);
	elm_object_style_set(button, "transparent");

	Evas_Object *image = elm_image_add(button);
	Eina_Bool res = elm_image_file_set(image, layoutFilePath.c_str(), GROUP_BUTTON_BACKSPACE);
	WARN_IF(res != EINA_TRUE, "elm_layout_file_set() failed");
	elm_object_part_content_set(button, "elm.swallow.content", image);

	return button;
}

void DialerView::onEntryChanged()
{
	std::string number = m_Entry->getNumber();
	m_SearchEngine.search(number);
	if (!number.empty()) {
		m_SearchControl->setResults(m_SearchEngine.getSearchResult());
	} else {
		m_SearchControl->clearResults();
	}
}

void DialerView::onResultSelected(SearchResultPtr result)
{
	if (result) {
		m_Entry->setNumber(result->getNumber(false));
	} else {
		Ui::ListPopup *popup = new Ui::ListPopup();
		popup->create(getEvasObject());
		popup->setTitle("IDS_KPD_BUTTON_ADD_TO_CONTACTS_ABB2");
		popup->addItem("IDS_LOGS_BUTTON_CREATE_CONTACT_ABB", [this] {
			m_AppControl = App::requestContactCreate(m_Entry->getNumber().c_str());
			m_AppControl.launch();
		});
		popup->addItem("IDS_LOGS_BUTTON_UPDATE_CONTACT_ABB2", [this] {
			m_AppControl = App::requestContactEdit(0, m_Entry->getNumber().c_str());
			m_AppControl.launch();
		});

		elm_popup_orient_set(popup->getEvasObject(), ELM_POPUP_ORIENT_CENTER);
	}
}

void DialerView::onKeyPressed(Ui::Button &button)
{
	KeypadButton &key = static_cast<KeypadButton &>(button);
	m_Entry->insert(key.getValue());
}

bool DialerView::onKeyLongpressed(Ui::Button &button)
{
	KeypadButton &key = static_cast<KeypadButton &>(button);
	int id = key.getId();

	if (m_Entry->getNumber().empty()) {
		if (id >= KeypadButton::ID_1 && id <= KeypadButton::ID_9) {
			launchSpeedDial(key.getValue() - '0');
			return true;
		}
	}

	if (id == KeypadButton::ID_0) {
		m_Entry->insert('+');
		return true;
	}

	return false;
}

void DialerView::onBackspacePressed(Ui::Button &button)
{
	feedback_play(FEEDBACK_PATTERN_KEY_BACK);
	m_Entry->popBack();
}

bool DialerView::onBackspaceLongpressed(Ui::Button &button)
{
	m_Entry->clear();
	return true;
}

void DialerView::onCallPressed(Evas_Object *obj, void *event_info)
{
	std::string number = m_Entry->getNumber();
	if (!number.empty()) {
		launchCall(number);
		m_Entry->clear();
	} else {
		m_Entry->setNumber(Phone::getLastCallNumber());
	}
}

void DialerView::onDbChanged(const char *uri)
{
	std::string number = m_Entry->getNumber();
	if (!number.empty()) {
		m_SearchEngine.searchFromScratch(number);
		m_SearchControl->setResults(m_SearchEngine.getSearchResult());
	}
}

void DialerView::launchCall(const std::string &number)
{
	App::AppControl request = App::requestTelephonyCall(number.c_str());
	request.launch(nullptr, nullptr, false);
	request.detach();
}

void DialerView::launchSpeedDial(int digit)
{
	std::string number = Phone::getSpeedDialNumber(digit);
	if (!number.empty()) {
		launchCall(number);
	} else {
		Ui::Popup *popup = new SpeedDialPopup(digit);
		popup->create(getEvasObject());
	}
}
