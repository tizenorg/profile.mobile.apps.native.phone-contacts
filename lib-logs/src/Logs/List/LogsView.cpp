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

#include "Ui/Genlist.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

using namespace Logs::Model;
using namespace Logs::List;

LogsView::LogsView(LogProvider::FilterType filterType)
	: m_LogProvider(filterType),
	  m_Genlist(nullptr),
	  m_Mode(ItemMode::Default)
{
}

Evas_Object *LogsView::onCreate(Evas_Object *parent)
{
	m_Genlist = new Ui::Genlist();
	m_Genlist->create(parent);

	fillGenlist();

	m_LogProvider.setInsertCallback([this](LogGroup *group){
		onLogInserted(group);
	});

	return m_Genlist->getEvasObject();
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
		/*
		 * TODO
		 */
	});

	menu->addItem("IDS_LOGS_OPT_DELETE", [this] {
		LogsView *deleteView = new LogsView();
		deleteView->setMode(ItemMode::Pick);
		getNavigator()->navigateTo(deleteView);
	});
	menu->show();

}

void LogsView::fillGenlist()
{
	if (!m_LogProvider.getLogGroupList().empty()) {
		LogGroupItem *groupItem = nullptr;

		for (auto &&group : m_LogProvider.getLogGroupList()) {
			groupItem = insertLogItem(group.get(), groupItem);
		}
	}
}

void LogsView::onLogInserted(LogGroup *group)
{
	LogGroupItem *groupItem = dynamic_cast<LogGroupItem *>(m_Genlist->getFirstItem());
	groupItem = insertLogItem(group, groupItem);
	groupItem->scrollTo(ELM_GENLIST_ITEM_SCROLLTO_TOP);
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

LogItem *LogsView::createLogItem(LogGroup *group)
{
	LogItem *item = new LogItem(group, m_Mode);

	item->setSelectedCallback([item, this]() {
		if (m_Mode == ItemMode::Default) {
			/*
			 * TODO
			 */
		} else {
			item->removeGroup();
		}
	});

	item->setDeleteCallback([this](LogItem *item) {
		LogGroupItem *itemGroup = static_cast<LogGroupItem *>(item->getParentItem());
		delete item;

		if (itemGroup->empty()) {
			delete itemGroup;
		}
	});

	return item;
}

void LogsView::setMode(ItemMode mode)
{
	m_Mode = mode;
}
