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

#include "Ui/GenContainer.h"
#include "Ui/GenItem.h"

using namespace Ui;

#define INSERTER_PARAMS Evas_Object *obj, const Elm_Gen_Item_Class *itc, const void *data, \
	Elm_Object_Item *parent, Elm_Object_Item *sibling, int type, \
	Evas_Smart_Cb func, const void *func_data

typedef Elm_Object_Item *(*Inserter)(INSERTER_PARAMS);
typedef void (*Updater)(Elm_Genlist_Item *obj, const char *parts, int type);

struct Ui::GenContainerApi
{
	Evas_Object *(*create)(Evas_Object *parent);
	Elm_Object_Item *(*getFirstItem)(const Evas_Object *obj);
	Elm_Object_Item *(*getLastItem)(const Evas_Object *obj);
	Eina_List *(*getRealizedItems)(const Evas_Object *obj);
	Updater updateItemFields;

	Inserter appendItem;
	Inserter prependItem;
	Inserter insertAfter;
	Inserter insertBefore;
};

namespace
{
	const GenContainerApi api[] = {
		{
			elm_genlist_add,
			elm_genlist_first_item_get,
			elm_genlist_last_item_get,
			elm_genlist_realized_items_get,
			(Updater) elm_genlist_item_fields_update,

			[](INSERTER_PARAMS) { return elm_genlist_item_append(obj, itc, data, parent, Elm_Genlist_Item_Type(type), func, func_data); },
			[](INSERTER_PARAMS) { return elm_genlist_item_prepend(obj, itc, data, parent, Elm_Genlist_Item_Type(type), func, func_data); },
			(Inserter) elm_genlist_item_insert_after,
			(Inserter) elm_genlist_item_insert_before
		},
		{
			elm_gengrid_add,
			elm_gengrid_first_item_get,
			elm_gengrid_last_item_get,
			elm_gengrid_realized_items_get,
			(Updater) elm_gengrid_item_fields_update,

			[](INSERTER_PARAMS) { return elm_gengrid_item_append(obj, itc, data, func, func_data); },
			[](INSERTER_PARAMS) { return elm_gengrid_item_prepend(obj, itc, data, func, func_data); },
			[](INSERTER_PARAMS) { return elm_gengrid_item_insert_after(obj, itc, data, sibling, func, func_data); },
			[](INSERTER_PARAMS) { return elm_gengrid_item_insert_before(obj, itc, data, sibling, func, func_data); },
		}
	};
}

GenContainer::GenContainer(Type type)
	: m_Api(&api[type])
{
}

Evas_Object *GenContainer::onCreate(Evas_Object *parent)
{
	Evas_Object *obj = m_Api->create(parent);

	evas_object_smart_callback_add(obj, "selected",
			(Evas_Smart_Cb) &GenContainer::onItemSelected, this);
	evas_object_smart_callback_add(obj, "longpressed",
			(Evas_Smart_Cb) &GenContainer::onItemLongpressed, this);
	evas_object_smart_callback_add(obj, "realized",
			(Evas_Smart_Cb) &GenContainer::onItemRealized, this);
	evas_object_smart_callback_add(obj, "unrealized",
			(Evas_Smart_Cb) &GenContainer::onItemUnrealized, this);
	return obj;
}

GenItem *GenContainer::getFirstItem() const
{
	Elm_Object_Item *item = m_Api->getFirstItem(getEvasObject());
	return (GenItem *) elm_object_item_data_get(item);
}

GenItem *GenContainer::getLastItem() const
{
	Elm_Object_Item *item = m_Api->getLastItem(getEvasObject());
	return (GenItem *) elm_object_item_data_get(item);
}

GenIterator GenContainer::begin() const
{
	return getFirstItem();
}

GenIterator GenContainer::end() const
{
	return nullptr;
}

void GenContainer::insert(GenItem *item, GenItem *parent, GenItem *sibling, Position position)
{
	if (!item) {
		return;
	}

	if (item->isInserted()) {
		item->pop();
	}

	Elm_Object_Item *parentItem = parent ? parent->getObjectItem() : nullptr;
	Elm_Object_Item *siblingItem = sibling ? sibling->getObjectItem() : nullptr;

	Inserter insert = nullptr;
	if (siblingItem) {
		insert = (position == Before) ? m_Api->insertBefore : m_Api->insertAfter;
	} else {
		insert = (position == Before) ? m_Api->appendItem : m_Api->prependItem;
	}

	item->onInserted(insert(getEvasObject(), item->getItemClass(), item,
					parentItem, siblingItem, item->getType(), nullptr, nullptr));
}

void GenContainer::update(const char *parts, int type)
{
	Eina_List *list = m_Api->getRealizedItems(getEvasObject());
	Eina_List *node = nullptr;
	void *item = nullptr;
	EINA_LIST_FOREACH(list, node, item) {
		m_Api->updateItemFields((Elm_Object_Item *) item, parts, type);
	}

	eina_list_free(list);
}

void GenContainer::onItemSelected(void *data, Evas_Object *obj, Elm_Object_Item *objectItem)
{
	GenItem *item = (GenItem *) elm_object_item_data_get(objectItem);
	if (item) {
		item->onSelected(objectItem);
	}
}

void GenContainer::onItemLongpressed(void *data, Evas_Object *obj, Elm_Object_Item *objectItem)
{
	GenItem *item = (GenItem *) elm_object_item_data_get(objectItem);
	if (item) {
		item->onLongpressed(objectItem);
	}
}

void GenContainer::onItemRealized(void *data, Evas_Object *obj, Elm_Object_Item *objectItem)
{
	GenItem *item = (GenItem *) elm_object_item_data_get(objectItem);
	if (item) {
		item->onRealized(objectItem);
	}
}

void GenContainer::onItemUnrealized(void *data, Evas_Object *obj, Elm_Object_Item *objectItem)
{
	GenItem *item = (GenItem *) elm_object_item_data_get(objectItem);
	if (item) {
		item->onUnrealized(objectItem);
	}
}
