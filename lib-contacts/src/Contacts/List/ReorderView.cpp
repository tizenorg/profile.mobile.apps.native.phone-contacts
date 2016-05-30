/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Contacts/List/Model/FavoritesProvider.h"
#include "Contacts/List/ListSection.h"
#include "Contacts/List/ReorderItem.h"
#include "Contacts/List/ReorderView.h"

#include "Common/Strings.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

using namespace Contacts::List;
using namespace Contacts::List::Model;

ReorderView::ReorderView()
	: m_CancelButton(nullptr), m_DoneButton(nullptr), m_Genlist(nullptr)
{}

Evas_Object *ReorderView::onCreate(Evas_Object *parent)
{
	m_Genlist = new Ui::Genlist();
	m_Genlist->create(parent);

	ListSection *favorites = new ListSection("IDS_PB_HEADER_FAVOURITES",
			new FavoritesProvider(), ListSection::ReorderMode);

	if (favorites && !favorites->empty() && !favorites->isInserted()) {
		m_Genlist->insert(favorites);
		elm_genlist_item_select_mode_set(favorites->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
	}
	evas_object_smart_callback_add(m_Genlist->getEvasObject(), "moved",
			makeCallback(&ReorderView::onReorder), this);

	return m_Genlist->getEvasObject();
}

void ReorderView::onPageAttached(Ui::NavigatorPage *page)
{
	RETM_IF(!page, "invalid page")

	page->setTitle("IDS_PB_OPT_REORDER");;

	createPageButtons();
	page->setContent("title_right_btn", m_DoneButton);
	page->setContent("title_left_btn", m_CancelButton);
}

void ReorderView::createPageButtons()
{
	m_CancelButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_CancelButton, "naviframe/title_left");
	elm_object_translatable_text_set(m_CancelButton, "IDS_PB_BUTTON_CANCEL");
	evas_object_smart_callback_add(m_CancelButton, "clicked",
			makeCallback(&ReorderView::onCancelPressed), this);

	m_DoneButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_DoneButton, "naviframe/title_right");
	elm_object_translatable_text_set(m_DoneButton, "IDS_PB_BUTTON_DONE_ABB3");
	evas_object_smart_callback_add(m_DoneButton, "clicked",
			makeCallback(&ReorderView::onDonePressed), this);
	elm_object_disabled_set(m_DoneButton, EINA_TRUE);
}

void ReorderView::onReorder(Evas_Object *obj, void *event_info)
{
	elm_object_disabled_set(m_DoneButton, EINA_FALSE);
	elm_genlist_reorder_mode_set(obj, EINA_FALSE);

	ReorderItem *item = (ReorderItem *)elm_object_item_data_get((Elm_Object_Item *)event_info);
	RETM_IF(!item, "item is invalid");
	int itemId = item->getContactData().getId();

	ReorderItem *prevItem = (ReorderItem *)item->getPrevItem();
	int prevItemId = 0;
	if (prevItem && elm_genlist_item_type_get(prevItem->getObjectItem())
			!= ELM_GENLIST_ITEM_GROUP) {
		prevItemId = prevItem->getContactData().getId();
	}

	ReorderItem *nextItem = (ReorderItem *)item->getNextItem();
	int nextItemId = 0;
	if (nextItem && elm_genlist_item_type_get(nextItem->getObjectItem())
			!= ELM_GENLIST_ITEM_GROUP) {
		nextItemId = nextItem->getContactData().getId();
	}

	ReorderData data;
	data.personId = itemId;
	data.prevPersonId = prevItemId;
	data.nextPersonId = nextItemId;

	m_ReorderDatas.push_back(data);
}

void ReorderView::onCancelPressed(Evas_Object *button, void *eventInfo)
{
	Ui::NavigatorPage *page = getPage();
	RETM_IF(!page, "invalid page")
	page->close();
}

void ReorderView::onDonePressed(Evas_Object *button, void *eventInfo)
{
	for (auto &&reorderData : m_ReorderDatas) {
		contacts_person_set_favorite_order(reorderData.personId,
				reorderData.prevPersonId, reorderData.nextPersonId);
	}

	Ui::NavigatorPage *page = getPage();
	RETM_IF(!page, "invalid page")
	page->close();
}
