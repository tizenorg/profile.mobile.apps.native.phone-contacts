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

#include "Ui/GenGroupItem.h"
#include <algorithm>
#include <app_i18n.h>

using namespace Ui;

GenGroupItem::GenGroupItem(const char *title)
{
	if (title) {
		m_Title = title;
	}
}

GenGroupItem::~GenGroupItem()
{
	for (auto &&itemPtr : m_ItemsCache) {
		if (auto item = itemPtr.lock()) {
			delete item.get();
		}
	}
}

GenItem *GenGroupItem::getFirstItem() const
{
	const Eina_List *subitems = elm_genlist_item_subitems_get(getObjectItem());
	Elm_Object_Item *item = (Elm_Object_Item *) eina_list_data_get(subitems);
	return (GenItem *) elm_object_item_data_get(item);
}

GenItem *GenGroupItem::getLastItem() const
{
	const Eina_List *subitems = elm_genlist_item_subitems_get(getObjectItem());
	Elm_Object_Item *item = (Elm_Object_Item *) eina_list_last_data_get(subitems);
	return (GenItem *) elm_object_item_data_get(item);
}

GenIterator GenGroupItem::begin()
{
	return getFirstItem();
}

GenIterator GenGroupItem::end()
{
	GenItem *lastItem = getLastItem();
	return lastItem ? lastItem->getNextItem() : nullptr;
}

size_t GenGroupItem::getItemCount() const
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

bool GenGroupItem::isEmpty() const
{
	return getItemCount() == 0;
}

bool GenGroupItem::isExpanded() const
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

GenGroupItem *GenGroupItem::getNextGroupItem() const
{
	GenItem *lastItem = getLastItem();
	GenItem *item = lastItem ? lastItem->getNextItem() : getNextItem();
	if (item && item->isGroupItem()) {
		return dynamic_cast<GenGroupItem *>(item);
	}

	return nullptr;
}

GenGroupItem *GenGroupItem::getPrevGroupItem() const
{
	GenItem *item = getPrevItem();
	if (item) {
		if (item->isGroupItem()) {
			return dynamic_cast<GenGroupItem *>(item);
		} else {
			return item->getParentItem();
		}
	}

	return nullptr;
}

void GenGroupItem::insertSubItem(GenItem *item, GenItem *sibling,
		GenContainer::Position position)
{
	if (!item) {
		return;
	}

	GenContainer *container = getParent();
	if (container && isExpanded()) {
		container->insert(item, this, sibling, position);
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
			if (position == GenContainer::After) {
				++pos;
			}
		} else {
			if (position == GenContainer::After) {
				pos = m_ItemsCache.begin();
			}
		}

		m_ItemsCache.insert(pos, item->getWeakPtr());
	}
}

Elm_Gen_Item_Class *GenGroupItem::getItemClass() const
{
	static Elm_Gen_Item_Class itc = createItemClass("group_index");
	return &itc;
}

char *GenGroupItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		return strdup(_(m_Title.c_str()));
	}

	return nullptr;
}

void GenGroupItem::onInserted()
{
	if (getType() == ELM_GENLIST_ITEM_GROUP) {
		elm_genlist_item_select_mode_set(getObjectItem(), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
	if (isExpanded()) {
		insertSubItems();
	}
}

void GenGroupItem::onPop()
{
	popSubItems();
}

void GenGroupItem::onExpanded(bool isExpanded)
{
	if (isExpanded) {
		insertSubItems();
	} else {
		popSubItems();
	}
}

void GenGroupItem::insertSubItems()
{
	GenContainer *container = getParent();
	if (container) {
		for (auto &&itemPtr : m_ItemsCache) {
			if (auto item = itemPtr.lock()) {
				container->insert(item.get(), this);
			}
		}
		m_ItemsCache.clear();
	}
}

void GenGroupItem::popSubItems()
{
	for (auto it = begin(), endIt = end(); it != endIt; ) {
		GenItem *item = *it++;
		item->pop();
		m_ItemsCache.push_back(item->getWeakPtr());
	}
}
