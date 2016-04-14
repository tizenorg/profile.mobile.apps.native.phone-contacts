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

#include "Ui/ListPopup.h"
#include <app_i18n.h>

using namespace Ui;

struct ListPopup::ItemData
{
	std::string text;
	void *userData;
	ItemSelectedCallback callback;
};

ListPopup::ListPopup(const char *itemStyle)
	: m_Genlist(nullptr), m_ItemClass{ ELM_GEN_ITEM_CLASS_HEADER, 0 }
{
	m_ItemClass.item_style = itemStyle;
	m_ItemClass.func.text_get = (Elm_Gen_Item_Text_Get_Cb) &ListPopup::getText;
	m_ItemClass.func.content_get = (Elm_Gen_Item_Content_Get_Cb) &ListPopup::getContent;
	m_ItemClass.func.del = [](void *data, Evas_Object *obj) {
		delete (ItemData *) data;
	};
}

Elm_Object_Item *ListPopup::addItem(std::string text, void *data, ItemSelectedCallback callback)
{
	ItemData *itemData = new ItemData{ std::move(text), data, std::move(callback) };
	return elm_genlist_item_append(m_Genlist, &m_ItemClass, itemData,
			nullptr, ELM_GENLIST_ITEM_NONE, nullptr, nullptr);
}

Elm_Object_Item *ListPopup::addItem(std::string text, ItemSelectedCallback callback)
{
	return addItem(std::move(text), nullptr, std::move(callback));
}

Elm_Object_Item *ListPopup::addItem(void *data, ItemSelectedCallback callback)
{
	return addItem("", data, std::move(callback));
}

void ListPopup::setSelectedCallback(SelectedCallback callback)
{
	m_OnSelected = std::move(callback);
}

Evas_Object *ListPopup::onCreate(Evas_Object *parent)
{
	Evas_Object *popup = Popup::onCreate(parent);

	createGenlist(parent);
	elm_object_part_content_set(popup, "elm.swallow.content", m_Genlist);

	return popup;
}

Evas_Object *ListPopup::createGenlist(Evas_Object *parent)
{
	m_Genlist = elm_genlist_add(parent);
	elm_genlist_homogeneous_set(m_Genlist, EINA_TRUE);
	elm_genlist_mode_set(m_Genlist, ELM_LIST_COMPRESS);
	elm_scroller_content_min_limit(m_Genlist, EINA_FALSE, EINA_TRUE);

	evas_object_smart_data_set(m_Genlist, this);
	evas_object_smart_callback_add(m_Genlist, "selected",
			(Evas_Smart_Cb) &ListPopup::onSelected, this);
	return m_Genlist;
}

char *ListPopup::getText(ItemData *itemData, Evas_Object *genlist, const char *part)
{
	ListPopup *popup = (ListPopup *) evas_object_smart_data_get(genlist);
	char *text = popup->getItemText(itemData->userData, part);
	if (!text && strcmp(part, "elm.text") == 0) {
		text = strdup(_(itemData->text.c_str()));
	}

	return text;
}

Evas_Object *ListPopup::getContent(ItemData *itemData, Evas_Object *genlist, const char *part)
{
	ListPopup *popup = (ListPopup *) evas_object_smart_data_get(genlist);
	return popup->getItemContent(itemData->userData, part);
}

void ListPopup::onSelected(ListPopup *popup, Evas_Object *genlist, Elm_Object_Item *item)
{
	ItemData *itemData = (ItemData *) elm_object_item_data_get(item);
	popup->onItemSelected(itemData->userData);

	if (itemData->callback) {
		itemData->callback();
	}

	if (popup->m_OnSelected) {
		popup->m_OnSelected(itemData->userData);
	}

	elm_genlist_item_selected_set(item, EINA_FALSE);

	/* FIXME: Destroying genlist from it's "selected" callback causes crash. */
	ecore_job_add([](void *data) {
		delete (ListPopup *) data;
	}, popup);
}
