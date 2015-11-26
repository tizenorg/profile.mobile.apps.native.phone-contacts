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

#include "Contacts/Settings/MainView.h"
#include "Contacts/Settings/SortByItem.h"
#include "Contacts/Settings/NameFormatItem.h"
#include "Ui/Genlist.h"

using namespace Contacts::Settings;

MainView::MainView()
	: m_Genlist(nullptr)
{
}

Evas_Object *MainView::onCreate(Evas_Object *parent)
{
	m_Genlist = new Ui::Genlist();
	m_Genlist->create(parent);
	m_Genlist->insert(new SortByItem());
	m_Genlist->insert(new NameFormatItem());

	return m_Genlist->getEvasObject();
}

void MainView::onPageAttached()
{
	getPage()->setTitle("IDS_PB_HEADER_SETTINGS");
}
