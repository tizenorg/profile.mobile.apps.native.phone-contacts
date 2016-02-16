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

#include "Logs/List/LogView.h"
#include "Logs/List/LogGroupItem.h"

#include "App/Path.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include "LogPath.h"

#include <algorithm>
#include <contacts.h>

using namespace Logs::Model;
using namespace Logs::List;
using namespace Ui;

LogView::LogView(LogProvider::FilterType filterType)
	: m_LogProvider(new Model::LogProvider(filterType)),
	  m_Genlist(nullptr),
	  m_Mode(LogItem::Mode::ModeDefault)
{
}

LogView::~LogView()
{
	delete m_LogProvider;
}

Evas_Object *LogView::onCreate(Evas_Object *parent)
{
	m_Genlist = new Ui::Genlist();
	m_Genlist->create(parent);

	fillGenlist();

	m_LogProvider->setInsertCallback([this](LogGroup *group){
		LogGroupItem *groupItem = dynamic_cast<LogGroupItem *>(m_Genlist->getFirstItem());
		insertLogGroupItem(group->getLogList().back(), &groupItem, Genlist::After);
		groupItem->scrollTo(ELM_GENLIST_ITEM_SCROLLTO_TOP);
	});

	return m_Genlist->getEvasObject();
}

void LogView::onPageAttached()
{
	getPage()->setTitle("IDS_LOGS_ITAB3_LOGS");
}

void LogView::onMenuPressed()
{
	if (m_Mode == LogItem::ModePick) {
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
		LogView *deleteView = new LogView();
		deleteView->setMode(LogItem::ModePick);
		getNavigator()->navigateTo(deleteView);
	});
	menu->show();

}

void LogView::fillGenlist()
{
	if (!m_LogProvider->getLogGroupList().empty()) {
		LogGroupItem *group = nullptr;

		auto it = m_LogProvider->getLogGroupList().rbegin();
		for (;it != m_LogProvider->getLogGroupList().rend(); it++) {
			insertLogGroupItem(it->get()->getLogList().back(), &group);
		}
	}
}

void LogView::insertLogGroupItem(const Log *log, LogGroupItem **group, Genlist::Position position)
{
	if (!*group || !compareTime((*group)->getDate(), log->getTime())) {
		*group = createLogGroupItem(log->getTime(), position);
		m_Genlist->insert(createLogItem(log->getLogGroup()), *group);
	} else {
		m_Genlist->insert(createLogItem(log->getLogGroup()), *group, nullptr, Genlist::After);
	}
}

LogGroupItem *LogView::createLogGroupItem(tm date, Genlist::Position position)
{
	LogGroupItem *groupItem = new LogGroupItem(date);
	m_Genlist->insert(groupItem, nullptr, nullptr, position);
	elm_genlist_item_select_mode_set(groupItem->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
	return groupItem;
}

LogItem *LogView::createLogItem(LogGroup *group)
{
	LogItem *item = new LogItem(group, m_Mode);

	item->setSelectedCallback([item, this]() {
		if (m_Mode == LogItem::ModeDefault) {
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

bool LogView::compareTime(const tm itemDate, const tm nowDate)
{
	if(itemDate.tm_year == nowDate.tm_year &&
		itemDate.tm_mon == nowDate.tm_mon &&
		itemDate.tm_mday == nowDate.tm_mday) {
		return true;
	}
	return false;
}

void LogView::setMode(LogItem::Mode mode)
{
	m_Mode = mode;
}
