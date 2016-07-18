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

#include "Logs/Details/DetailsView.h"
#include "Logs/Details/BasicInfoItem.h"
#include "Logs/Details/ActionItem.h"
#include "Logs/Details/LogDetailItem.h"
#include "Logs/List/LogGroupItem.h"
#include "Logs/Model/Log.h"
#include "LogsDetailsItemLayout.h"
#include "Common/Strings.h"

#include "App/Path.h"
#include "System/Settings.h"
#include "Ui/Genlist.h"
#include "Ui/Menu.h"
#include "Utils/Callback.h"

#include <algorithm>

using namespace Logs::Model;
using namespace Logs::Details;
using namespace Logs::List;

using namespace Ux;
using namespace System;
using namespace std::placeholders;

DetailsView::DetailsView(const char *number)
	: m_Group(nullptr)
	, m_LogProvider(number)
	, m_Genlist(nullptr)
	, m_BasicInfoItem(nullptr)
	, m_ActionItem(nullptr)
	, m_LastGroupItem(nullptr)
{
	auto strings = Common::getSelectViewStrings();
	strings.buttonDone = "IDS_TPLATFORM_ACBUTTON_DELETE_ABB";
	strings.titleMulti = "IDS_CLOG_HEADER_SELECT_LOGS";
	setStrings(strings);

	setSelectCallback(std::bind(&DetailsView::onSelected, this, _1));
	setCancelCallback(std::bind(&DetailsView::onCanceled, this));

	Settings::addCallback(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR,
			{ makeCallback(&DetailsView::onTimeFormatChanged), this });
	Settings::addCallback(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY,
			{ makeCallback(&DetailsView::onTimeFormatChanged), this });
}

DetailsView::~DetailsView()
{
	Settings::removeCallback(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR,
			{ makeCallback(&DetailsView::onTimeFormatChanged), this });
	Settings::removeCallback(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY,
			{ makeCallback(&DetailsView::onTimeFormatChanged), this });
}

Evas_Object *DetailsView::onCreate(Evas_Object *parent)
{
	elm_theme_extension_add(nullptr, App::getResourcePath(LOGS_DETAILS_ITEM_LAYOUT_EDJ).c_str());

	m_Genlist = new Ui::Genlist();
	return m_Genlist->create(parent);
}

void DetailsView::onCreated()
{
	m_LogProvider.setInsertCallback(std::bind(&DetailsView::onLogGroupInserted, this, _1));
	m_Group = m_LogProvider.getLogGroupList().back().get();
	fillGenList();
}

void DetailsView::onMenuPressed()
{
	if (getSelectMode() != SelectNone) {
		return;
	}

	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());
	menu->addItem("IDS_LOGS_OPT_DELETE", [this] {
		setSelectMode(SelectMulti);
	});
	menu->show();
}

void DetailsView::onSelectModeChanged(SelectMode selectMode)
{
	if (getSelectMode() == SelectNone) {
		insertBasicInfoItem();
		insertActionItem();
	} else {
		if (m_BasicInfoItem) {
			delete m_BasicInfoItem;
			m_BasicInfoItem = nullptr;
		}
		if (m_ActionItem) {
			delete m_ActionItem;
			m_ActionItem = nullptr;
		}
	}
}

void DetailsView::onSelectAllInsert(Ui::GenItem *item)
{
	m_Genlist->insert(item, nullptr, nullptr, Ui::Genlist::After);
}

void DetailsView::fillGenList()
{
	if (getSelectMode() == SelectNone) {
		insertBasicInfoItem();
		insertActionItem();
	}
	insertLogGroupList();
}

void DetailsView::insertBasicInfoItem()
{
	m_BasicInfoItem = new BasicInfoItem(m_Group);
	m_BasicInfoItem->setBackCallback([this] {
		getPage()->close();
	});
	m_Genlist->insert(m_BasicInfoItem, nullptr, nullptr, Ui::Genlist::After);
}

void DetailsView::insertActionItem()
{
	m_ActionItem = new ActionItem(m_Group);
	m_Genlist->insert(m_ActionItem, nullptr, m_BasicInfoItem, Ui::Genlist::After);
	elm_genlist_item_select_mode_set(m_ActionItem->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
}

void DetailsView::insertLogGroupList()
{
	for (auto &&group : m_LogProvider.getLogGroupList()) {
		insertLogGroupItem(group.get());
		insertLogDetailItems(group.get());
	}
}

void DetailsView::insertLogGroupItem(LogGroup *group)
{
	Log *log = group->getLogList().back();
	LogGroupItem *groupItem = new LogGroupItem(log->getTime());
	m_Genlist->insert(groupItem, nullptr, getLastGroupItem());
	elm_genlist_item_select_mode_set(groupItem->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
	setLastGroupItem(groupItem);
	group->setLogAddCallback(std::bind(&DetailsView::onLogAdded, this, _1));
	group->addChangeCallback(std::bind(&DetailsView::onGroupChanged, this, group, groupItem, _1));
}

void DetailsView::insertLogDetailItems(LogGroup *group)
{
	for (auto &log : group->getLogList()) {
		insertLogDetailItem(log);
	}
}

void DetailsView::insertLogDetailItem(Log *log)
{
	LogDetailItem *logItem = new LogDetailItem(log);
	m_Genlist->insert(logItem, getLastGroupItem(), *getLastGroupItem()->begin());
	log->setRemoveCallback(std::bind(&DetailsView::onLogRemoved, this, logItem));
	addSelectItem(logItem);
}

LogGroupItem *DetailsView::getLastGroupItem()
{
	if (!m_LastGroupItem) {
		for (auto &&item : *m_Genlist) {
			if (((Ui::GenItem *)item)->isGroupItem()) {
				setLastGroupItem(dynamic_cast<LogGroupItem *>(item));
				break;
			}
		}
	}

	return m_LastGroupItem;
}

void DetailsView::setLastGroupItem(LogGroupItem *groupItem)
{
	if (m_LastGroupItem) {
		m_LastGroupItem->setDestroyCallback(nullptr);
	}

	m_LastGroupItem = groupItem;
	m_LastGroupItem->setDestroyCallback([this] {
		m_LastGroupItem = nullptr;
	});
}

bool DetailsView::onSelected(SelectResults results)
{
	for (auto &&result : results) {
		Log *log = (Log *)result.value.data;
		log->remove();
	}
	setSelectMode(SelectNone);
	return false;
}

bool DetailsView::onCanceled()
{
	setSelectMode(SelectNone);
	return false;
}

void DetailsView::onTimeFormatChanged(system_settings_key_e key)
{
	m_Genlist->update(PART_LOG_TIME, ELM_GENLIST_ITEM_FIELD_TEXT);
}

void DetailsView::onLogGroupInserted(LogGroup *group)
{
	if (m_Genlist) {
		insertLogGroupItem(group);
		insertLogDetailItems(group);
	}
}

void DetailsView::onGroupChanged(LogGroup *group, LogGroupItem *groupItem, int type)
{
	if (type & LogGroup::ChangeRemoved) {
		if (m_LogProvider.getLogGroupList().empty()) {
			m_BasicInfoItem->updateGroup(nullptr);
			m_ActionItem->updateGroup(nullptr);
			delete m_BasicInfoItem;
			delete m_ActionItem;
			getPage()->close();
			return;
		}
		delete groupItem;
		if (m_Group == group) {
			m_Group = m_LogProvider.getLogGroupList().back().get();
			m_BasicInfoItem->updateGroup(m_Group);
			m_ActionItem->updateGroup(m_Group);
		}
	}
}

void DetailsView::onLogAdded(Log *log)
{
	insertLogDetailItem(log);
}

void DetailsView::onLogRemoved(LogDetailItem *logItem)
{
	removeSelectItem(logItem);
	delete logItem;

}
