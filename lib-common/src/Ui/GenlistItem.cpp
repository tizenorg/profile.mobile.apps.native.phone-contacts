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

#include "Ui/GenlistItem.h"
#include "Ui/GenlistGroupItem.h"
#include "Ui/Genlist.h"
#include "Utils/Callback.h"

using namespace Ui;

namespace
{
	Elm_Genlist_Item_Class defaultItemClass = GenlistItem::createItemClass("type1");
}

GenlistItem::GenlistItem(Elm_Genlist_Item_Class *itemClass, Elm_Genlist_Item_Type itemType)
	: m_ItemType(itemType), m_ItemClass(itemClass), m_Item(nullptr), m_Preserve(false)
{
	if (!m_ItemClass) {
		m_ItemClass = &defaultItemClass;
	}
}

GenlistItem::~GenlistItem()
{
	if (m_Item) {
		pop();
	}
}

Elm_Genlist_Item_Class GenlistItem::createItemClass(const char *style,
		const char *decorateStyle, const char *editStyle)
{
	Elm_Genlist_Item_Class itc = { ELM_GEN_ITEM_CLASS_HEADER, 0 };
	itc.item_style = style;
	itc.decorate_item_style = decorateStyle;
	itc.decorate_all_item_style = editStyle;
	itc.func.text_get = makeCallback(&GenlistItem::getText);
	itc.func.content_get = makeCallback(&GenlistItem::getContent);
	itc.func.state_get = makeCallback(&GenlistItem::getState);
	itc.func.del = makeCallback(&GenlistItem::onDestroy);

	return itc;
}

bool GenlistItem::isGroupItem() const
{
	return false;
}

Elm_Genlist_Item_Type GenlistItem::getType() const
{
	return m_ItemType;
}

Elm_Object_Item *GenlistItem::getObjectItem() const
{
	return m_Item;
}

Genlist *GenlistItem::getParent() const
{
	Evas_Object *genlist = elm_object_item_widget_get(getObjectItem());
	return static_cast<Genlist *>(Control::getControl(genlist));
}

GenlistGroupItem *GenlistItem::getParentItem() const
{
	Elm_Object_Item *item = elm_genlist_item_parent_get(getObjectItem());
	return (GenlistGroupItem *) elm_object_item_data_get(item);
}

GenlistItem *GenlistItem::getNextItem() const
{
	Elm_Object_Item *item = elm_genlist_item_next_get(getObjectItem());
	return (GenlistItem *) elm_object_item_data_get(item);
}

GenlistItem *GenlistItem::getPrevItem() const
{
	Elm_Object_Item *item = elm_genlist_item_prev_get(getObjectItem());
	return (GenlistItem *) elm_object_item_data_get(item);
}

void GenlistItem::pop()
{
	onPop();
	m_Preserve = true;
	elm_object_item_del(m_Item);
	m_Preserve = false;
}

void GenlistItem::onInserted(Elm_Object_Item *item)
{
	m_Item = item;
	GenlistGroupItem *parent = getParentItem();
	if (parent) {
		parent->onSubItemInserted(this);
	}
	onInserted();
}

void GenlistItem::onDestroy(Evas_Object *genlist)
{
	GenlistGroupItem *parent = getParentItem();
	if (parent) {
		parent->onSubItemDestroy(this);
	}

	m_Item = nullptr;
	if (!m_Preserve) {
		delete this;
	}
}
