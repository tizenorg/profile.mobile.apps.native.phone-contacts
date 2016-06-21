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

#include "Contacts/List/RemoveFavView.h"
#include "Contacts/List/ListSection.h"

#include "Contacts/List/Model/FavoritesProvider.h"
#include "Contacts/List/Model/MfcProvider.h"
#include "Contacts/List/PersonItem.h"

#include "Common/Strings.h"

#include "Ui/Genlist.h"
#include "Utils/Logger.h"

using namespace Contacts::List;
using namespace Contacts::List::Model;

using namespace std::placeholders;

RemoveFavView::RemoveFavView()
	: m_Genlist(nullptr), m_Sections{ nullptr }
{
	auto strings = Common::getSelectViewStrings();
	strings.titleMulti = "IDS_PB_HEADER_SELECT_CONTACT_ABB2";
	setStrings(strings);
}

Evas_Object *RemoveFavView::onCreate(Evas_Object *parent)
{
	m_Genlist = new Ui::Genlist();
	m_Genlist->create(parent);

	createSections();
	setSelectMode(Ux::SelectMulti);

	return m_Genlist->getEvasObject();
}

void RemoveFavView::onDestroy()
{
	for (auto &&section : m_Sections) {
		if (section && !section->isInserted()) {
			delete section;
		}
	}
}

void RemoveFavView::onSelectAllInsert(Ui::GenlistItem *item)
{
	m_Genlist->insert(item, nullptr, nullptr, Ui::Genlist::After);
}

Ui::GenlistGroupItem *RemoveFavView::createListSection(SectionId sectionId)
{
	const char *title = nullptr;
	PersonProvider *provider = nullptr;
	switch (sectionId) {
		case SectionFavorites:
			title = "IDS_PB_HEADER_FAVOURITES";
			provider = new FavoritesProvider(FavoritesProvider::ModeOnly);
			break;
		case SectionMfc:
			title = "IDS_PB_HEADER_MOST_FREQUENT_CONTACTS_ABB2";
			provider = new MfcProvider();
			break;
		default:
			return nullptr;
	}

	ListSection *section = new ListSection(title, provider);
	section->setUpdateCallback(std::bind(&RemoveFavView::onSectionUpdated, this, _1, _2, sectionId));

	if (!section->isEmpty()) {
		insertSection(section, sectionId);
		for (auto &&item : *section) {
			addSelectItem(static_cast<PersonItem *>(item));
		}
	}

	return section;
}

void RemoveFavView::createSections()
{
	for (size_t i = SectionFirst; i < SectionMax; ++i) {
		m_Sections[i] = createListSection(static_cast<SectionId>(i));;
	}
}

void RemoveFavView::insertSection(Ui::GenlistGroupItem *section, SectionId sectionId)
{
	m_Genlist->insert(section, nullptr, getNextSectionItem(sectionId));
	elm_genlist_item_select_mode_set(section->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
}

void RemoveFavView::updateSection(SectionId sectionId)
{
	Ui::GenlistGroupItem *section = m_Sections[sectionId];

	if (!section->isEmpty()) {
		if (!section->isInserted()) {
			insertSection(section, sectionId);
		}
	} else {
		if (section->isInserted()) {
			section->pop();
		}
	}
}

void RemoveFavView::updateSections()
{
	for (size_t i = SectionFirst; i < SectionMax; ++i) {
		updateSection(static_cast<SectionId>(i));
	}
}

Ui::GenlistItem *RemoveFavView::getNextSectionItem(SectionId sectionId)
{
	for (unsigned section = sectionId + 1; section < SectionMax; ++section) {
		if (m_Sections[section] && m_Sections[section]->isInserted()) {
			return m_Sections[section];
		}
	}

	return nullptr;
}

void RemoveFavView::onSectionUpdated(ContactItem *item, ::Common::ChangeType change, SectionId sectionId)
{
	if (change == Common::ChangeInsert) {
		addSelectItem(item);
	} else if (change == Common::ChangeDelete) {
		removeSelectItem(item);
	}

	updateSection(sectionId);
}
