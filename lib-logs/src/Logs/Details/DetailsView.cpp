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
#include "Ui/Genlist.h"
#include "Ui/Menu.h"

#include <algorithm>

using namespace Logs::Model;
using namespace Logs::Details;
using namespace Logs::List;

using namespace Ux;
using namespace std::placeholders;

DetailsView::DetailsView(Log *log)
	: m_Log(log)
	, m_LogProvider(nullptr)
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

	m_LogProvider = new NumberLogProvider(m_Log->getNumber());
}

DetailsView::~DetailsView()
{
	for (auto &&group : m_LogProvider->getLogGroupList()) {
			group->unsetLogAddCallback();
			for (auto &log : group->getLogList()) {
				log->unsetRemoveCallback();
			}
	}

	delete m_LogProvider;
}

Evas_Object *DetailsView::onCreate(Evas_Object *parent)
{
	elm_theme_extension_add(nullptr, App::getResourcePath(LOGS_DETAILS_ITEM_LAYOUT_EDJ).c_str());

	m_Genlist = new Ui::Genlist();
	return m_Genlist->create(parent);
}

void DetailsView::onCreated()
{
	m_LogProvider->setInsertCallback(std::bind(&DetailsView::onLogGroupInserted, this, _1));
	fillGenList();
}

bool DetailsView::onBackPressed()
{
	if (getSelectMode() != SelectNone) {
		setSelectMode(SelectNone);
		return false;
	}
	return true;
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

void DetailsView::onSelectAllInsert(Ui::GenlistItem *item)
{
	m_Genlist->insert(item, nullptr, nullptr, Ui::Genlist::After);
}

void DetailsView::fillGenList()
{
	if (getSelectMode() == SelectNone) {
		insertBasicInfoItem();
		insertActionItem();
	}
	insertGroupAndLogDetailItems();
}

void DetailsView::insertBasicInfoItem()
{
	m_BasicInfoItem = new BasicInfoItem(m_Log);
	m_BasicInfoItem->setBackCallback([this] {
		getPage()->close();
	});
	m_Genlist->insert(m_BasicInfoItem, nullptr, nullptr, Ui::Genlist::After);
}

void DetailsView::insertActionItem()
{
	m_ActionItem = new ActionItem(m_Log);
	m_Genlist->insert(m_ActionItem, nullptr, m_BasicInfoItem, Ui::Genlist::After);
	elm_genlist_item_select_mode_set(m_ActionItem->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
}

void DetailsView::insertGroupAndLogDetailItems()
{
	for (auto &&group : m_LogProvider->getLogGroupList()) {
		insertGroupItems(group.get());
	}
}

void DetailsView::insertGroupItems(LogGroup *group)
{
	group->setLogAddCallback(std::bind(&DetailsView::onLogAdded, this, _1));
	group->addChangeCallback(std::bind(&DetailsView::onGroupChanged, this, _1));
	Log *log = group->getLogList().back();
	insertLogGroupItem(log->getTime());
	insertLogDetailItems(group);
}


void DetailsView::insertLogGroupItem(tm date)
{
	LogGroupItem *groupItem = new LogGroupItem(date);
	m_Genlist->insert(groupItem, nullptr, getLastGroupItem());
	elm_genlist_item_select_mode_set(groupItem->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
	setLastGroupItem(groupItem);
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
	onItemInserted(logItem);
}

LogGroupItem *DetailsView::getLastGroupItem()
{
	if (!m_LastGroupItem) {
		for (auto &&item : *m_Genlist) {
			if (item->isGroupItem()) {
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

void DetailsView::onLogGroupInserted(LogGroup *group)
{
	if (m_Genlist) {
		insertGroupItems(group);
	}
}

void DetailsView::onGroupChanged(int type) {
	if ((type & LogGroup::ChangeRemoved) && m_LogProvider->getLogGroupList().empty()) {
		getPage()->close();
		return;
	} else {
		m_Log = m_LogProvider->getLogGroupList().back().get()->getLogList().back();
		m_BasicInfoItem->updateLog(m_Log, type);
		m_ActionItem->updateLog(m_Log, type);
	}
}

void DetailsView::onLogAdded(Log *log)
{
	insertLogDetailItem(log);
}

void DetailsView::onLogRemoved(LogDetailItem *logItem)
{
	Ui::GenlistGroupItem *groupItem = logItem->getParentItem();

	onItemRemove(logItem);
	delete logItem;

	if (groupItem && groupItem->isEmpty()) {;
		delete groupItem;
	}
}
