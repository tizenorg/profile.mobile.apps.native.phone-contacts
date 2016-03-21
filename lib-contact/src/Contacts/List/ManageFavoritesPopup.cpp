/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Contacts/List/ListView.h"
#include "Contacts/List/ManageFavoritesPopup.h"

#include "Ui/Navigator.h"

using namespace Contacts::List;
using namespace Ui;

ManageFavoritesPopup::ManageFavoritesPopup(Navigator *navigator)
	: m_Navigator(navigator)
{
}

void ManageFavoritesPopup::onCreated()
{
	ListPopup::onCreated();

	setTitle("IDS_PB_HEADER_MANAGE_FAVOURITES_ABB");
	setBackCallback( [this] {
		return true;
	});

	addItem("IDS_PB_OPT_ADD", std::bind(&ManageFavoritesPopup::onAdd, this));
	addItem("IDS_PB_OPT_REORDER", std::bind(&ManageFavoritesPopup::onReorder, this));
	addItem("IDS_PB_OPT_REMOVE", std::bind(&ManageFavoritesPopup::onRemove, this));
}

void ManageFavoritesPopup::onAdd()
{
	ListView *view = new ListView();
	view->setSelectMode(SelectMulti);
	view->setSelectCallback([](SelectResults results) {
		//todo
		return true;
	});
	m_Navigator->navigateTo(view);
}

void ManageFavoritesPopup::onReorder()
{
	//todo
}

void ManageFavoritesPopup::onRemove()
{
	//todo
}
