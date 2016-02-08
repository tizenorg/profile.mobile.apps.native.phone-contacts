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

#include "Ui/Genlist.h"
#include "Ui/GenlistGroupItem.h"

using namespace Ui;

Evas_Object *Genlist::onCreate(Evas_Object *parent)
{
	Evas_Object *genlist = elm_genlist_add(parent);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

	evas_object_smart_callback_add(genlist, "selected",
			(Evas_Smart_Cb) &Genlist::onItemSelected, this);
	evas_object_smart_callback_add(genlist, "expanded",
			(Evas_Smart_Cb) &Genlist::onItemExpanded, this);
	evas_object_smart_callback_add(genlist, "contracted",
			(Evas_Smart_Cb) &Genlist::onItemContracted, this);
	evas_object_smart_callback_add(genlist, "realized",
			(Evas_Smart_Cb) &Genlist::onItemRealized, this);
	evas_object_smart_callback_add(genlist, "unrealized",
			(Evas_Smart_Cb) &Genlist::onItemUnrealized, this);

	return genlist;
}

GenlistItem *Genlist::getFirstItem() const
{
	Elm_Object_Item *item = elm_genlist_first_item_get(getEvasObject());
	return (GenlistItem *) elm_object_item_data_get(item);
}

GenlistItem *Genlist::getLastItem() const
{
	Elm_Object_Item *item = elm_genlist_last_item_get(getEvasObject());
	return (GenlistItem *) elm_object_item_data_get(item);
}

GenlistIterator Genlist::begin() const
{
	return getFirstItem();
}

GenlistIterator Genlist::end() const
{
	return nullptr;
}

void Genlist::insert(GenlistItem *item, GenlistGroupItem *parent,
		GenlistItem *sibling, Position position)
{
	if (!item) {
		return;
	}

	if (item->m_Item) {
		item->pop();
	}

	Elm_Object_Item *parentItem = parent ? parent->getObjectItem() : nullptr;
	if (sibling) {
		auto insert = (position == Before) ? elm_genlist_item_insert_before : elm_genlist_item_insert_after;
		item->onInserted(insert(getEvasObject(), item->getItemClass(), item,
				parentItem, sibling->getObjectItem(), item->getType(), nullptr, nullptr));
	} else {
		auto insert = (position == Before) ? elm_genlist_item_append : elm_genlist_item_prepend;
		item->onInserted(insert(getEvasObject(), item->getItemClass(), item,
				parentItem, item->getType(), nullptr, nullptr));
	}
}

void Genlist::update(const char *parts, Elm_Genlist_Item_Field_Type type)
{
	Eina_List *list = elm_genlist_realized_items_get(getEvasObject());
	Eina_List *node = nullptr;
	void *item = nullptr;
	EINA_LIST_FOREACH(list, node, item) {
		elm_genlist_item_fields_update((Elm_Object_Item *) item, parts, type);
	}

	eina_list_free(list);
}

void Genlist::onItemSelected(void *data, Evas_Object *obj, Elm_Object_Item *objectItem)
{
	GenlistItem *item = (GenlistItem *) elm_object_item_data_get(objectItem);
	if (item) {
		item->onSelected();
		elm_genlist_item_selected_set(objectItem, EINA_FALSE);
	}
}

void Genlist::onItemExpanded(void *data, Evas_Object *obj, Elm_Object_Item *objectItem)
{
	GenlistItem *item = (GenlistItem *) elm_object_item_data_get(objectItem);
	if (item && item->isGroupItem()) {
		dynamic_cast<GenlistGroupItem *>(item)->onExpanded(true);
	}
}

void Genlist::onItemContracted(void *data, Evas_Object *obj, Elm_Object_Item *objectItem)
{
	GenlistItem *item = (GenlistItem *) elm_object_item_data_get(objectItem);
	if (item && item->isGroupItem()) {
		dynamic_cast<GenlistGroupItem *>(item)->onExpanded(false);
	}
}

void Genlist::onItemRealized(void *data, Evas_Object *obj, Elm_Object_Item *objectItem)
{
	GenlistItem *item = (GenlistItem *) elm_object_item_data_get(objectItem);
	if (item) {
		item->onRealized(objectItem);
	}
}

void Genlist::onItemUnrealized(void *data, Evas_Object *obj, Elm_Object_Item *objectItem)
{
	GenlistItem *item = (GenlistItem *) elm_object_item_data_get(objectItem);
	if (item) {
		item->onUnrealized(objectItem);
	}
}
