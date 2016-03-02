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
#include "Logs/Model/LogGroup.h"
#include "Logs/Details/BasicInfoItem.h"

#include "App/Path.h"
#include "Ui/Genlist.h"
#include "LogsDetailsItemLayout.h"

using namespace Logs::Model;
using namespace Logs::Details;

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
	m_BasicInfoItem = new BasicInfoItem(m_Group);
	m_Genlist->insert(m_BasicInfoItem);
}

void DetailsView::onPageAttached()
{
	Ui::NavigatorPage *page = getPage();
	page->setTitle(nullptr);
}
