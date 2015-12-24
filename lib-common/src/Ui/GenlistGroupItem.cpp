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

GenlistGroupItem::GenlistGroupItem()
	: m_FirstItem(nullptr), m_LastItem(nullptr)
{
}

GenlistGroupItem::~GenlistGroupItem()
{
	for (auto &&item : m_ItemsCache) {
		delete item;
	}
}

GenlistIterator GenlistGroupItem::begin()
{
	return m_FirstItem;
}

GenlistIterator GenlistGroupItem::end()
{
	return m_LastItem ? m_LastItem->getNextItem() : nullptr;
}

bool GenlistGroupItem::empty() const
{
	return m_FirstItem == nullptr;
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
	GenlistItem *item = m_LastItem ? m_LastItem->getNextItem() : getNextItem();
	if (item && item->isGroupItem()) {
		return dynamic_cast<GenlistGroupItem *>(item);
	}

	return nullptr;
}

GenlistGroupItem *GenlistGroupItem::getPrevGroupItem() const
{
	GenlistItem *item = getPrevItem();
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

	Genlist *genlist = getParent();
	if (genlist && isExpanded()) {
		genlist->insert(item, this, sibling, position);
	} else {
		auto pos = m_ItemsCache.end();
		if (sibling) {
			pos = std::find(m_ItemsCache.begin(), m_ItemsCache.end(), sibling);
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

		m_ItemsCache.insert(pos, item);
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

void GenlistGroupItem::onExpanded()
{
	insertSubItems();
}

void GenlistGroupItem::onContracted()
{
	popSubItems();
}

void GenlistGroupItem::insertSubItems()
{
	Genlist *genlist = getParent();
	if (genlist) {
		for (auto &&item : m_ItemsCache) {
			genlist->insert(item, this);
		}
		m_ItemsCache.clear();
	}
}

void GenlistGroupItem::popSubItems()
{
	for (auto it = begin(), endIt = end(); it != endIt; ) {
		GenlistItem *item = *it++;
		item->pop();
		m_ItemsCache.push_back(item);
	}
}

void GenlistGroupItem::onSubItemInserted(GenlistItem *item)
{
	if (!m_FirstItem) {
		m_FirstItem = m_LastItem = item;
		return;
	}

	if (item->getNextItem() == m_FirstItem) {
		m_FirstItem = item;
	} else if (item->getPrevItem() == m_LastItem) {
		m_LastItem = item;
	}
}

void GenlistGroupItem::onSubItemDestroy(GenlistItem *item)
{
	if (m_FirstItem == m_LastItem) {
		m_FirstItem = m_LastItem = nullptr;
		return;
	}

	if (item == m_FirstItem) {
		m_FirstItem = item->getNextItem();
	} else if (item == m_LastItem) {
		m_LastItem = item->getPrevItem();
	}
}
