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

#include "Phone/Dialer/MainView.h"
#include "Phone/Dialer/AddNumberPopup.h"
#include "Phone/Dialer/KeypadButton.h"
#include "Phone/Dialer/KeypadEntry.h"
#include "Phone/Dialer/SearchResultsControl.h"
#include "Phone/Dialer/SpeedDialPopup.h"
#include "Phone/SpeedDial/SettingsView.h"
#include "Phone/Utils.h"

#include "App/AppControlRequest.h"
#include "App/Path.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Ui/Window.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include "CommonButtons.h"
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

MainView::MainView()
	: m_Entry(nullptr), m_SearchWidget(nullptr)
{
}

MainView::~MainView()
{
	feedback_deinitialize();
	contacts_db_remove_changed_cb(_contacts_contact._uri,
			makeCallbackWithLastParam(&MainView::onDbChanged), this);
}

void MainView::onCreated()
{
	feedback_initialize();
	contacts_db_add_changed_cb(_contacts_contact._uri,
			makeCallbackWithLastParam(&MainView::onDbChanged), this);
}

Evas_Object *MainView::onCreate(Evas_Object *parent)
{
	elm_theme_extension_add(nullptr, App::getResourcePath(COMMON_BUTTONS_EDJ).c_str());

	Evas_Object *layout = elm_layout_add(parent);
	Eina_Bool res = elm_layout_file_set(layout, layoutFilePath.c_str(), GROUP_DIALER);
	WARN_IF(res != EINA_TRUE, "elm_layout_file_set() failed");

	elm_object_part_content_set(layout, PART_SWALLOW_ENTRY, createEntry(layout));
	elm_object_part_content_set(layout, PART_SWALLOW_PREDICTIVE, createSearchWidget(layout));
	elm_object_part_content_set(layout, PART_SWALLOW_KEYPAD, createKeypad(layout));
	elm_object_part_content_set(layout, PART_SWALLOW_CALL, createCallButton(layout));
	elm_object_part_content_set(layout, PART_SWALLOW_BACKSPACE, createBackspaceButton(layout));

	return layout;
}

void MainView::setNumber(const std::string &number)
{
	if (m_Entry) {
		m_Entry->setNumber(number);
	}
}

void MainView::onPageAttached()
{
	getPage()->setTitle("IDS_KPD_ITAB3_KEYPAD");
}

void MainView::onNavigation(bool isCurrentView)
{
	Evas_Object *conf = getWindow()->getConformant();
	if (isCurrentView) {
		elm_object_signal_emit(conf, "elm,state,virtualkeypad,disable", "");
		elm_object_signal_emit(conf, "elm,state,clipboard,disable", "");
	} else {
		elm_object_signal_emit(conf, "elm,state,virtualkeypad,enable", "");
		elm_object_signal_emit(conf, "elm,state,clipboard,enable", "");
	}
}

Evas_Object *MainView::onMenuPressed()
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

	return menu->getEvasObject();
}

Evas_Object *MainView::createEntry(Evas_Object *parent)
{
	m_Entry = new KeypadEntry();
	m_Entry->setChangedCallback(std::bind(&MainView::onEntryChanged, this));
	return m_Entry->create(parent);
}

Evas_Object *MainView::createSearchWidget(Evas_Object *parent)
{
	m_SearchWidget = new SearchResultsControl();
	m_SearchWidget->setSelectedCallback(std::bind(&MainView::onResultSelected, this, _1));
	return m_SearchWidget->create(parent);
}

Evas_Object *MainView::createKeypad(Evas_Object *parent)
{
	Evas_Object *table = elm_table_add(parent);
	elm_table_padding_set(table, 2, 2);

	int id = KeypadButton::ID_1;
	for (int i = 0; i < KEYPAD_ROWS; ++i) {
		for (int j = 0; j < KEYPAD_COLS; ++j, ++id) {
			KeypadButton *key = new KeypadButton((KeypadButton::Id) id);
			key->setPressedCallback(std::bind(&MainView::onKeyPressed, this, _1));
			key->setLongpressedCallback(std::bind(&MainView::onKeyLongpressed, this, _1));

			Evas_Object *button = key->create(table);
			evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
			elm_table_pack(table, button, j, i, 1, 1);
			evas_object_show(button);
		}
	}

	return table;
}

Evas_Object *MainView::createCallButton(Evas_Object *parent)
{
	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, BUTTON_STYLE_CUSTOM_CIRCLE);
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&MainView::onCallPressed), this);

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

Evas_Object *MainView::createBackspaceButton(Evas_Object *parent)
{
	Ui::Button *key = new Ui::Button();
	key->setPressedCallback(std::bind(&MainView::onBackspacePressed, this, _1));
	key->setLongpressedCallback(std::bind(&MainView::onBackspaceLongpressed, this, _1));

	Evas_Object *button = key->create(parent);
	elm_object_style_set(button, "transparent");

	Evas_Object *image = elm_image_add(button);
	Eina_Bool res = elm_image_file_set(image, layoutFilePath.c_str(), GROUP_BUTTON_BACKSPACE);
	WARN_IF(res != EINA_TRUE, "elm_layout_file_set() failed");
	elm_object_part_content_set(button, "elm.swallow.content", image);

	return button;
}

void MainView::onEntryChanged()
{
	std::string number = m_Entry->getNumber();
	if (!number.empty()) {
		m_SearchEngine.search(number);
		m_SearchWidget->setResults(m_SearchEngine.getSearchResult());
	} else {
		m_SearchWidget->clearResults();
	}
}

void MainView::onResultSelected(SearchResultPtr result)
{
	if (result) {
		m_Entry->setNumber(result->getNumber(false));
	} else {
		Ui::Popup *popup = new AddNumberPopup(m_Entry->getNumber());
		popup->create(getEvasObject());
	}
}

void MainView::onKeyPressed(Ui::Button &button)
{
	KeypadButton &key = static_cast<KeypadButton &>(button);
	m_Entry->insert(key.getValue());
}

bool MainView::onKeyLongpressed(Ui::Button &button)
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

void MainView::onBackspacePressed(Ui::Button &button)
{
	feedback_play(FEEDBACK_PATTERN_KEY_BACK);
	m_Entry->popBack();
}

bool MainView::onBackspaceLongpressed(Ui::Button &button)
{
	m_Entry->clear();
	return true;
}

void MainView::onCallPressed(Evas_Object *obj, void *event_info)
{
	std::string number = m_Entry->getNumber();
	if (!number.empty()) {
		launchCall(number);
		m_Entry->clear();
	} else {
		m_Entry->setNumber(Phone::getLastCallNumber());
	}
}

void MainView::onDbChanged(const char *uri)
{
	std::string number = m_Entry->getNumber();
	m_SearchEngine.searchFromScratch(number);
	m_SearchWidget->setResults(m_SearchEngine.getSearchResult());
}

void MainView::launchCall(const std::string &number)
{
	App::AppControl request = App::requestTelephonyCall(number.c_str());
	request.launch(nullptr, nullptr, false);
	request.detach();
}

void MainView::launchSpeedDial(int digit)
{
	std::string number = Phone::getSpeedDialNumber(digit);
	if (!number.empty()) {
		launchCall(number);
	} else {
		Ui::Popup *popup = new SpeedDialPopup(digit);
		popup->create(getEvasObject());
	}
}
