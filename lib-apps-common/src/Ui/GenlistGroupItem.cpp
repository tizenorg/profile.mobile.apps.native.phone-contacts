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

#include "Ui/GenlistGroupItem.h"
#include "Ui/Genlist.h"
#include <algorithm>

using namespace Ui;

GenlistGroupItem::~GenlistGroupItem()
{
	for (auto &&itemPtr : m_ItemsCache) {
		if (auto item = itemPtr.lock()) {
			delete item.get();
		}
	}
}

GenlistItem *GenlistGroupItem::getFirstItem() const
{
	const Eina_List *subitems = elm_genlist_item_subitems_get(getObjectItem());
	Elm_Object_Item *item = (Elm_Object_Item *) eina_list_data_get(subitems);
	return (GenlistItem *) elm_object_item_data_get(item);
}

GenlistItem *GenlistGroupItem::getLastItem() const
{
	const Eina_List *subitems = elm_genlist_item_subitems_get(getObjectItem());
	Elm_Object_Item *item = (Elm_Object_Item *) eina_list_last_data_get(subitems);
	return (GenlistItem *) elm_object_item_data_get(item);
}

GenIterator GenlistGroupItem::begin()
{
	return getFirstItem();
}

GenIterator GenlistGroupItem::end()
{
	GenlistItem *lastItem = getLastItem();
	return lastItem ? lastItem->getNextItem() : nullptr;
}

size_t GenlistGroupItem::getItemCount() const
{
	if (!m_ItemsCache.empty()) {
		return std::count_if(m_ItemsCache.begin(), m_ItemsCache.end(),
			[](const GenItemPtr &itemPtr) {
				return !itemPtr.expired();
			});
	}

	const Eina_List *subitems = elm_genlist_item_subitems_get(getObjectItem());
	return eina_list_count(subitems);
}

bool GenlistGroupItem::isEmpty() const
{
	if (!m_ItemsCache.empty()) {
		return !std::any_of(m_ItemsCache.begin(), m_ItemsCache.end(),
			[](const GenItemPtr &itemPtr) {
				return !itemPtr.expired();
			});
	}

	const Eina_List *subitems = elm_genlist_item_subitems_get(getObjectItem());
	return eina_list_count(subitems) == 0;
}

bool GenlistGroupItem::isExpanded() const
{
	Elm_Object_Item *item = getObjectItem();
	if (!item) {
		return false;
	}

	if (getType() == ELM_GENLIST_ITEM_TREE) {
		return elm_genlist_item_expanded_get(item);
	}

	return true;
}

GenlistGroupItem *GenlistGroupItem::getNextGroupItem() const
{
	GenlistItem *lastItem = getLastItem();
	GenlistItem *item = static_cast<GenlistItem *>(lastItem ? lastItem->getNextItem() : getNextItem());
	if (item && item->isGroupItem()) {
		return dynamic_cast<GenlistGroupItem *>(item);
	}

	return nullptr;
}

GenlistGroupItem *GenlistGroupItem::getPrevGroupItem() const
{
	GenlistItem *item = static_cast<GenlistItem *>(getPrevItem());
	if (item) {
		if (item->isGroupItem()) {
			return dynamic_cast<GenlistGroupItem *>(item);
		} else {
			return item->getParentItem();
		}
	}

	return nullptr;
}

void GenlistGroupItem::insertSubItem(GenlistItem *item, GenlistItem *sibling,
		Genlist::Position position)
{
	if (!item) {
		return;
	}

	Genlist *genlist = static_cast<Genlist *>(getParent());
	if (genlist && isExpanded()) {
		genlist->insert(item, this, sibling, position);
	} else {
		auto pos = m_ItemsCache.end();
		if (sibling) {
			pos = std::find_if(m_ItemsCache.begin(), m_ItemsCache.end(),
				[sibling](GenItemPtr &itemPtr) {
					if (auto item = itemPtr.lock()) {
						return item.get() == sibling;
					}
					return false;
				});
		}

		if (pos != m_ItemsCache.end()) {
			if (position == Genlist::After) {
				++pos;
			}
		} else {
			if (position == Genlist::After) {
				pos = m_ItemsCache.begin();
			}
		}

		m_ItemsCache.insert(pos, item->getWeakPtr());
	}
}

Elm_Genlist_Item_Class *GenlistGroupItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass("group_index");
	return &itc;
}

void GenlistGroupItem::onInserted()
{
	if (isExpanded()) {
		insertSubItems();
	}
}

void GenlistGroupItem::onPop()
{
	popSubItems();
}

void GenlistGroupItem::onExpanded(bool isExpanded)
{
	if (isExpanded) {
		insertSubItems();
	} else {
		popSubItems();
	}
}

void GenlistGroupItem::insertSubItems()
{
	Genlist *genlist = static_cast<Genlist *>(getParent());
	if (genlist) {
		for (auto &&itemPtr : m_ItemsCache) {
			if (auto item = itemPtr.lock()) {
				genlist->insert(item.get(), this);
			}
		}
		m_ItemsCache.clear();
	}
}

void GenlistGroupItem::popSubItems()
{
	for (auto it = begin(), endIt = end(); it != endIt; ) {
		GenlistItem *item = static_cast<GenlistItem *>(*it++);
		item->pop();
		m_ItemsCache.push_back(item->getWeakPtr());
	}
}
