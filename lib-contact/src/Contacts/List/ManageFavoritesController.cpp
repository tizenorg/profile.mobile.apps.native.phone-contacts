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
#include "Contacts/List/ManageFavoritesController.h"

#include "Ui/ListPopup.h"
#include "Ui/Navigator.h"

#include <functional>

using namespace Contacts::List;
using namespace Ui;

ManageFavoritesController::ManageFavoritesController(Navigator *navigator)
	: m_Navigator(navigator), m_Popup(nullptr)
{
}

ManageFavoritesController::~ManageFavoritesController()
{
	delete m_Popup;
}

void ManageFavoritesController::showPopup()
{
	m_Popup = new ListPopup();
	m_Popup->create(m_Navigator->getEvasObject());
	m_Popup->setTitle("IDS_PB_HEADER_MANAGE_FAVOURITES_ABB");
	m_Popup->setBackCallback( [this] {
		delete this;
		return false;
	});

	m_Popup->addItem("IDS_PB_OPT_ADD", std::bind(&ManageFavoritesController::onAdd, this));
	m_Popup->addItem("IDS_PB_OPT_REORDER", std::bind(&ManageFavoritesController::onReorder, this));
	m_Popup->addItem("IDS_PB_OPT_REMOVE", std::bind(&ManageFavoritesController::onRemove, this));
}

void ManageFavoritesController::onAdd()
{
	ListView *view = new ListView();
	view->setSelectMode(SelectMulti);
	view->setSelectCallback([](SelectResults results) {

		return true;
	});
	m_Navigator->navigateTo(view);

	m_Popup = nullptr;
	delete this;
}

void ManageFavoritesController::onReorder()
{
	m_Popup = nullptr;
	delete this;
}

void ManageFavoritesController::onRemove()
{
	m_Popup = nullptr;
	delete this;
}
