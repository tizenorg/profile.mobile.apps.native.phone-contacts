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
	: m_Mode(ItemMode::Default), m_FilterType(filterType),
	  m_Genlist(nullptr), m_LastGroupItem(nullptr)
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

	fillLayout();
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

void LogsView::fillLayout()
{
	if (m_Genlist) {
		elm_genlist_clear(m_Genlist->getEvasObject());
	} else {
		updateLayout(false);
	}

	fillGenlist();

	if (!elm_genlist_items_count(m_Genlist->getEvasObject())) {
		updateLayout(true);
	}
}

void LogsView::updateLayout(bool isEmpty)
{
	Evas_Object *layout = nullptr;
	if (isEmpty) {
		layout = createNoContentsLayout(getEvasObject());
		delete m_Genlist;
		m_Genlist = nullptr;
	} else {
		m_Genlist = new Ui::Genlist();
		layout = m_Genlist->create(getEvasObject());
	}

	elm_object_part_content_set(getEvasObject(), "elm.swallow.content", layout);
}

Evas_Object *LogsView::createNoContentsLayout(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "nocontents", "default");
	elm_object_translatable_part_text_set(layout, "elm.text", "IDS_CLOG_BODY_NO_LOGS");
	elm_object_translatable_part_text_set(layout, "elm.help.text", "IDS_LOGS_BODY_AFTER_YOU_MAKE_OR_RECEIVE_CALLS_THEY_WILL_BE_LOGGED_HERE");

	elm_layout_signal_emit(layout, "align.center", "elm");

	return layout;
}

void LogsView::fillGenlist()
{
	for (auto &&group : m_LogProvider.getLogGroupList()) {
		if (shouldDisplayLogs(group.get())) {
			insertLogItem(group.get());
		}
	}

	auto item = m_Genlist->getFirstItem();
	if (item) {
		item->scrollTo();
	}
}

bool LogsView::shouldDisplayLogs(const LogGroup *group) const
{
	switch (m_FilterType) {
		case FilterMissed:
		{
			int type = group->getLogList().back()->getType();
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

LogItem *LogsView::insertLogItem(LogGroup *group)
{
	Log *log = group->getLogList().back();
	LogGroupItem *groupItem = getLastGroupItem();
	if (!groupItem || !LogProvider::compareDate(groupItem->getDate(), log->getTime())) {
		groupItem = insertGroupItem(log->getTime());
	}

	LogItem *item = createLogItem(group);
	m_Genlist->insert(item, groupItem, *groupItem->begin());
	return item;
}

LogGroupItem *LogsView::insertGroupItem(tm date)
{
	LogGroupItem *groupItem = new LogGroupItem(date);
	m_Genlist->insert(groupItem, nullptr, getLastGroupItem());
	elm_genlist_item_select_mode_set(groupItem->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);

	setLastGroupItem(groupItem);
	return groupItem;
}

LogGroupItem *LogsView::getLastGroupItem()
{
	if (!m_LastGroupItem) {
		for (auto &&item : *m_Genlist) {
			if (item->isGroupItem()) {
				m_LastGroupItem = dynamic_cast<LogGroupItem *>(item);
				break;
			}
		}
	}

	return m_LastGroupItem;
}

void LogsView::setLastGroupItem(LogGroupItem *groupItem)
{
	if (m_LastGroupItem) {
		m_LastGroupItem->setDestroyCallback(nullptr);
	}

	m_LastGroupItem = groupItem;
	m_LastGroupItem->setDestroyCallback([this] {
		m_LastGroupItem = nullptr;
	});
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
		auto filter = (FilterType)(long)data;
		if (m_FilterType != filter) {
			m_FilterType = filter;
			fillLayout();
		}
	});
}

void LogsView::onLogInserted(LogGroup *group)
{
	if (shouldDisplayLogs(group)) {
		if (!m_Genlist) {
			updateLayout(false);
		}

		insertLogItem(group);
		m_Genlist->getFirstItem()->scrollTo();
	}
}

void LogsView::onLogItemDelete(LogItem *item)
{
	Ui::GenlistGroupItem *groupItem = item->getParentItem();
	delete item;

	if (groupItem->empty()) {
		delete groupItem;
	}

	if (!elm_genlist_items_count(m_Genlist->getEvasObject())) {
		updateLayout(true);
	}
}
