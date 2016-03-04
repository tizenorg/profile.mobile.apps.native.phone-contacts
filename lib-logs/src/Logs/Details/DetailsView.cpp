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
#include "Logs/Details/LogDetailItem.h"
#include "Logs/List/LogGroupItem.h"
#include "Logs/Model/LogGroup.h"
#include "Logs/Model/Log.h"
#include "LogsDetailsItemLayout.h"

#include "App/Path.h"
#include "Ui/Genlist.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"

using namespace Logs::Model;
using namespace Logs::Details;
using namespace Logs::List;
using namespace Contacts;

DetailsView::DetailsView(LogGroup *group)
	: m_Group(group), m_Genlist(nullptr), m_BasicInfoItem(nullptr)
{
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

void DetailsView::onSelectModeChanged(SelectMode selectMode)
{
	if (getSelectMode() == SelectNone) {
		insertBasicInfoItem();
	} else if (m_BasicInfoItem) {
		delete m_BasicInfoItem;
	}
}

void DetailsView::onSelectAllInsert(Ui::GenlistItem *item)
{
	m_Genlist->insert(item, nullptr, nullptr, Ui::Genlist::After);
}

void DetailsView::onMenuPressed()
{
	if (getSelectMode() != SelectNone) {
		return;
	}

	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());

	menu->addItem("IDS_LOGS_OPT_DELETE", [this] {
		DetailsView *view = new DetailsView(m_Group);
		view->setSelectMode(SelectMulti);
		view->setSelectCallback([](SelectResults results) {
			for (auto &&result : results) {
				Log *log = (Log *)result.value.data;
				log->remove();
			}
			return true;
		});
		getNavigator()->navigateTo(view);
	});
	menu->show();
}

void DetailsView::fillGenList()
{
	if (getSelectMode() == SelectNone) {
		insertBasicInfoItem();
	}
	insertLogGroupItem();
	insertLogDetailItems();
}

void DetailsView::insertBasicInfoItem()
{
	m_BasicInfoItem = new BasicInfoItem(m_Group);
	m_BasicInfoItem->setBackCallback([this] {
		delete this;
	});
	m_Genlist->insert(m_BasicInfoItem, nullptr, nullptr, Ui::Genlist::After);
}

void DetailsView::insertLogGroupItem()
{
	Log *log = m_Group->getLogList().back();
	LogGroupItem *groupItem = new LogGroupItem(log->getTime());
	m_Genlist->insert(groupItem);
	elm_genlist_item_select_mode_set(groupItem->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
}

void DetailsView::insertLogDetailItems()
{
	for (auto &log:m_Group->getLogList()) {
		LogDetailItem *logItem = new LogDetailItem(log);
		m_Genlist->insert(logItem);
		onItemInserted(logItem);
	}
}
