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

using namespace Ui;

namespace
{
	Elm_Genlist_Item_Class groupItemClass = GenlistItem::createItemClass("group_index");
}

GenlistGroupItem::GenlistGroupItem(Elm_Genlist_Item_Class *itemClass,
		Elm_Genlist_Item_Type itemType)
	: GenlistItem(itemClass ? itemClass : &groupItemClass, itemType),
	  m_FirstItem(nullptr), m_LastItem(nullptr)
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

bool GenlistGroupItem::empty()
{
	return m_FirstItem == nullptr;
}

void GenlistGroupItem::onExpanded()
{
	Genlist *genlist = getParent();
	if (genlist) {
		for (auto &&item : m_ItemsCache) {
			genlist->insert(item, this);
		}
		m_ItemsCache.clear();
	}
}

void GenlistGroupItem::onContracted()
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
