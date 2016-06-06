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

#include "Common/Database/RecordIterator.h"
#include "Common/Database/RecordUtils.h"
#include "Contacts/List/ListView.h"
#include "Contacts/List/ManageFavoritesPopup.h"
#include "Contacts/List/Model/FavoritesProvider.h"
#include "Contacts/List/ReorderView.h"
#include "Ui/Navigator.h"

using namespace Common::Database;
using namespace Contacts::List;
using namespace Contacts::List::Model;
using namespace Ui;

ManageFavoritesPopup::ManageFavoritesPopup(Navigator *navigator)
	: m_Navigator(navigator)
{
}

void ManageFavoritesPopup::onCreated()
{
	ListPopup::onCreated();

	setTitle("IDS_PB_HEADER_MANAGE_FAVOURITES_ABB");

	// todo Implement get list's size without retrieve of all fav persons from database
	FavoritesProvider provider;
	int count = provider.getDataList().size();

	addItem("IDS_PB_OPT_ADD", std::bind(&ManageFavoritesPopup::onAdd, this));

	if (count > 0) {
		addItem("IDS_PB_OPT_REMOVE", std::bind(&ManageFavoritesPopup::onRemove, this));

		if (count > 1) {
			addItem("IDS_PB_OPT_REORDER", std::bind(&ManageFavoritesPopup::onReorder, this));
		}
	}
}

void ManageFavoritesPopup::onAdd()
{
	ListView *view = new ListView(new FavoritesProvider(FavoritesProvider::ModeExclude));
	view->setSelectMode(Ux::SelectMulti);
	view->setSelectCallback([](Ux::SelectResults results) {

		contacts_list_h list = nullptr;
		contacts_list_create(&list);
		for (auto &&result : results) {
			contacts_record_h record = nullptr;
			contacts_db_get_record(_contacts_person._uri, result.value.id, &record);
			contacts_record_set_bool(record, _contacts_person.is_favorite, true);
			contacts_list_add(list, record);
		}

		contacts_db_update_records(list);
		contacts_list_destroy(list, true);

		return true;
	});
	m_Navigator->navigateTo(view);
}

void ManageFavoritesPopup::onReorder()
{
	m_Navigator->navigateTo(new ReorderView());
}

void ManageFavoritesPopup::onRemove()
{
	//todo Should be created ListView with Favorites and MFC sections only
	ListView *view = new ListView();
	view->setSelectMode(Ux::SelectMulti);
	view->setSelectCallback([](Ux::SelectResults results) {

		contacts_list_h list = nullptr;
		contacts_list_create(&list);
		for (auto &&result : results) {
			contacts_record_h record = nullptr;
			int recordId = result.value.id;

			contacts_db_get_record(_contacts_person._uri, recordId, &record);
			if (getRecordBool(record, _contacts_person.is_favorite)) {
				contacts_record_set_bool(record, _contacts_person.is_favorite, false);
				contacts_list_add(list, record);
			} else {
				contacts_person_reset_usage(recordId, CONTACTS_USAGE_STAT_TYPE_OUTGOING_CALL);
				contacts_record_destroy(record, true);
			}
		}

		contacts_db_update_records(list);
		contacts_list_destroy(list, true);

		return true;
	});
	m_Navigator->navigateTo(view);
}
