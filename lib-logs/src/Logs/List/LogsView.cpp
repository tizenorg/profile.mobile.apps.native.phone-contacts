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

#include "Logs/List/LogsView.h"
#include "Logs/List/LogGroupItem.h"
#include "Logs/List/LogItem.h"
#include "Logs/Details/DetailsView.h"

#include "App/AppControlRequest.h"

#include "Ui/Genlist.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Ui/RadioPopup.h"

#include "Utils/Logger.h"
#include "Utils/Callback.h"

using namespace Logs::Model;
using namespace Logs::List;
using namespace Logs::Details;
using namespace std::placeholders;

LogsView::LogsView(FilterType filterType)
	: m_LogProvider(),
	  m_Genlist(nullptr),
	  m_Mode(ItemMode::Default),
	  m_FilterType(filterType)
{
	system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR, makeCallbackWithLastParam(&LogsView::onSettingsChanged), this);
	system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY, makeCallbackWithLastParam(&LogsView::onSettingsChanged), this);
	system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_TIME_CHANGED, makeCallbackWithLastParam(&LogsView::onSettingsChanged), this);
}

LogsView::~LogsView()
{
	system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR);
	system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY);
	system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_TIME_CHANGED);
}

Evas_Object *LogsView::onCreate(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "default");
	return layout;
}

void LogsView::onCreated()
{
	m_LogProvider.setInsertCallback(std::bind(&LogsView::onLogInserted, this, _1));
	updateLayout(m_LogProvider.getLogGroupList().empty());

	if (m_Genlist) {
		fillGenlist();
	}
}

void LogsView::onPageAttached()
{
	getPage()->setTitle("IDS_LOGS_ITAB3_LOGS");
}

void LogsView::onMenuPressed()
{
	if (m_Mode == ItemMode::Pick) {
		return;
	}

	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());

	menu->addItem("IDS_CLOG_OPT_VIEW_BY", [this] {
		onSelectViewBy();
	});

	menu->addItem("IDS_LOGS_OPT_DELETE", [this] {
		LogsView *deleteView = new LogsView(m_FilterType);
		deleteView->setMode(ItemMode::Pick);
		getNavigator()->navigateTo(deleteView);
	});
	menu->show();
}

void LogsView::updateLayout(bool isEmpty)
{
	Evas_Object *layout = nullptr;
	if (isEmpty) {
		layout = createNoContentsLayout(getEvasObject());
	} else {
		m_Genlist = new Ui::Genlist();
		layout = m_Genlist->create(getEvasObject());
	}

	elm_object_part_content_set(getEvasObject(), "elm.swallow.content", layout);
}

Evas_Object *LogsView::createNoContentsLayout(Evas_Object *parent)
{
	Evas_Object *noContentLayout = elm_layout_add(parent);
	elm_layout_theme_set(noContentLayout, "layout", "nocontents", "default");
	evas_object_size_hint_weight_set(noContentLayout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(noContentLayout, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_object_translatable_part_text_set(noContentLayout, "elm.text", "IDS_CLOG_BODY_NO_LOGS");
	elm_object_translatable_part_text_set(noContentLayout, "elm.help.text", "IDS_LOGS_BODY_AFTER_YOU_MAKE_OR_RECEIVE_CALLS_THEY_WILL_BE_LOGGED_HERE");

	elm_layout_signal_emit(noContentLayout, "text,disabled", "");
	elm_layout_signal_emit(noContentLayout, "align.center", "elm");

	return noContentLayout;
}

void LogsView::fillGenlist()
{
	LogGroupItem *groupItem = nullptr;

	for (auto &&group : m_LogProvider.getLogGroupList()) {
		if (shouldDisplayLogs(*group)) {
			groupItem = insertLogItem(group.get(), groupItem);
		}
	}

	groupItem = dynamic_cast<LogGroupItem *>(m_Genlist->getFirstItem());
	groupItem->scrollTo(ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

bool LogsView::shouldDisplayLogs(const LogGroup &group)
{
	switch (m_FilterType) {
		case FilterMissed:
		{
			int type = group.getLogList().back()->getType();
			return (type == CONTACTS_PLOG_TYPE_VOICE_INCOMING_SEEN ||
					type == CONTACTS_PLOG_TYPE_VOICE_INCOMING_UNSEEN);
		}
			break;
		default:
			return true;
	}
}

LogItem *LogsView::createLogItem(LogGroup *group)
{
	LogItem *item = new LogItem(group, m_Mode);

	item->setDeleteCallback(std::bind(&LogsView::onLogItemDelete, this, _1));
	item->setSelectCallback([this, group]() {
		const char *number = group->getLogList().back()->getNumber();
		if (number) {
			App::AppControl appControl = App::requestTelephonyCall(number);
			appControl.launch(nullptr, nullptr, false);
			appControl.detach();
		}
	});

	item->setDetailsCallback([this](LogItem *item) {
		getNavigator()->navigateTo(new DetailsView(item->getGroup()));
	});

	return item;
}

LogGroupItem *LogsView::insertLogItem(LogGroup *group, LogGroupItem *groupItem)
{
	Log *log = group->getLogList().back();

	if (!groupItem || !LogProvider::compareDate(groupItem->getDate(), log->getTime())) {
		groupItem = createLogGroupItem(log->getTime());
	}

	m_Genlist->insert(createLogItem(group), groupItem, (*groupItem->begin()));
	return groupItem;
}

LogGroupItem *LogsView::createLogGroupItem(tm date)
{
	LogGroupItem *groupItem = new LogGroupItem(date);
	m_Genlist->insert(groupItem, nullptr, nullptr, Ui::Genlist::After);
	elm_genlist_item_select_mode_set(groupItem->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
	return groupItem;
}

void LogsView::setMode(ItemMode mode)
{
	m_Mode = mode;
}

void LogsView::onSettingsChanged(system_settings_key_e key)
{
	if (key == SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY ||
			key == SYSTEM_SETTINGS_KEY_TIME_CHANGED) {
		elm_genlist_clear(m_Genlist->getEvasObject());
		m_LogProvider.resetLogGroups();
		fillGenlist();
	} else {
		m_Genlist->update(PART_LOG_TIME, ELM_GENLIST_ITEM_FIELD_TEXT);
	}
}

void LogsView::onSelectViewBy()
{
	Ui::RadioPopup *popup = new Ui::RadioPopup();
	popup->create(getEvasObject());
	popup->setTitle("IDS_CLOG_OPT_VIEW_BY");
	popup->addItem("IDS_LOGS_BODY_ALL_CALLS", (void *) FilterAll);
	popup->addItem("IDS_LOGS_OPT_MISSED_CALLS", (void *) FilterMissed);
	popup->setSelectedItem(m_FilterType);
	popup->setSelectedCallback([this](void *data) {
		if (m_FilterType != (FilterType)(long)data) {
			elm_genlist_clear(m_Genlist->getEvasObject());
			m_FilterType = (FilterType)(long)data;
			fillGenlist();
		}
	});
}

void LogsView::onLogInserted(LogGroup *group)
{
	if (shouldDisplayLogs(*group)) {
		if (!m_Genlist) {
			updateLayout(false);
		}

		LogGroupItem *groupItem = dynamic_cast<LogGroupItem *>(m_Genlist->getFirstItem());
		groupItem = insertLogItem(group, groupItem);
		groupItem->scrollTo(ELM_GENLIST_ITEM_SCROLLTO_TOP);
	}
}

void LogsView::onLogItemDelete(LogItem *item)
{
	LogGroupItem *itemGroup = static_cast<LogGroupItem *>(item->getParentItem());
	delete item;
	if (itemGroup->empty()) {
		delete itemGroup;
	}

	if (!elm_genlist_items_count(m_Genlist->getEvasObject())) {
		updateLayout(true);
	}
}
