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
#include "Utils/Callback.h"

using namespace Ui;

namespace
{
	Elm_Genlist_Item_Class defaultItemClass = GenlistItem::createItemClass("type1");
}

GenlistItem::GenlistItem(Elm_Genlist_Item_Class *itemClass, Elm_Genlist_Item_Type itemType)
	: m_ItemType(itemType), m_ItemClass(itemClass), m_Item(nullptr)
{
	if (!m_ItemClass) {
		m_ItemClass = &defaultItemClass;
	}
}

Elm_Genlist_Item_Class GenlistItem::createItemClass(const char *style,
		const char *decorateStyle, const char *editStyle)
{
	return {
		ELM_GEN_ITEM_CLASS_HEADER, EINA_FALSE,
		style, decorateStyle, editStyle,
		{
			makeCallback(&GenlistItem::getText),
			makeCallback(&GenlistItem::getContent),
			nullptr,
			[] (void *data, Evas_Object *obj) {
				delete (GenlistItem *) data;
			}
		}
	};
}

Elm_Object_Item *GenlistItem::getObjectItem() const
{
	return m_Item;
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

void GenlistItem::onContracted()
{
	elm_genlist_item_subitems_clear(getObjectItem());
}
