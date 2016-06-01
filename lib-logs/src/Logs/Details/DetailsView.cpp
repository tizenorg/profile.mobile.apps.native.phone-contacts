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

DetailsView::DetailsView(LogGroup *group)
	: m_Group(group), m_Genlist(nullptr), m_BasicInfoItem(nullptr), m_ActionItem(nullptr),
	  m_GroupItem(nullptr)
{
	auto strings = Common::getSelectViewStrings();
	strings.buttonDone = "IDS_TPLATFORM_ACBUTTON_DELETE_ABB";
	strings.titleMulti = "IDS_CLOG_HEADER_SELECT_LOGS";
	setStrings(strings);

	setSelectCallback(std::bind(&DetailsView::onSelected, this, _1));
	setCancelCallback(std::bind(&DetailsView::onCanceled, this));
	m_Group->setLogAddCallback(std::bind(&DetailsView::onLogAdded, this, _1));
	m_GroupChangeCbHandle = m_Group->addChangeCallback(std::bind(&DetailsView::onGroupChanged, this, _1));
}

DetailsView::~DetailsView()
{
	if (m_Group) {
		for (auto &log : m_Group->getLogList()) {
			log->unsetRemoveCallback();
		}
		m_Group->unsetLogAddCallback();
		m_Group->removeChangeCallback(m_GroupChangeCbHandle);
	}
}

Evas_Object *DetailsView::onCreate(Evas_Object *parent)
{
	elm_theme_extension_add(nullptr, App::getResourcePath(LOGS_DETAILS_ITEM_LAYOUT_EDJ).c_str());

	m_Genlist = new Ui::Genlist();
	return m_Genlist->create(parent);
}

void DetailsView::onCreated()
{
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
	insertLogGroupItem();
	insertLogDetailItems();
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

void DetailsView::insertLogGroupItem()
{
	Log *log = m_Group->getLogList().back();
	m_GroupItem = new LogGroupItem(log->getTime());
	m_Genlist->insert(m_GroupItem);
	elm_genlist_item_select_mode_set(m_GroupItem->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
}

void DetailsView::insertLogDetailItem(Log *log)
{
	LogDetailItem *logItem = new LogDetailItem(log);
	m_Genlist->insert(logItem, m_GroupItem, m_GroupItem, Ui::Genlist::After);
	log->setRemoveCallback(std::bind(&DetailsView::onLogRemoved, this, logItem));
	onItemInserted(logItem);
}

void DetailsView::insertLogDetailItems()
{
	for (auto &log : m_Group->getLogList()) {
		insertLogDetailItem(log);
	}
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

void DetailsView::onGroupChanged(int type)
{
	if (type & LogGroup::ChangeRemoved) {
		getPage()->close();
		m_Group = nullptr;
	}
}

void DetailsView::onLogAdded(Log *log)
{
	insertLogDetailItem(log);
}

void DetailsView::onLogRemoved(LogDetailItem *logItem)
{
	onItemRemove(logItem);
	delete logItem;
}
