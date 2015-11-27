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

#include "Contacts/Settings/RadioPopup.h"
#include "Utils/Logger.h"
#include <app_i18n.h>

using namespace Contacts::Settings;

struct RadioItemData
{
	std::string text;
	int value;
};

RadioPopup::RadioPopup(SelectedCallback callback)
	: m_Callback(std::move(callback)), m_Genlist(nullptr), m_ItemClass(nullptr), m_RadioGroup(nullptr)
{
}

RadioPopup::~RadioPopup()
{
	elm_genlist_item_class_free(m_ItemClass);
}

void RadioPopup::addItem(std::string text, int value)
{
	RadioItemData *itemData = new RadioItemData{ std::move(text), value };

	elm_genlist_item_append(m_Genlist, m_ItemClass, itemData, nullptr,
				ELM_GENLIST_ITEM_NONE, onSelect, this);
}

void RadioPopup::setSelectedItem(int value)
{
	elm_radio_value_set(m_RadioGroup, value);
}

void RadioPopup::onCreated()
{
	elm_popup_orient_set(getEvasObject(), ELM_POPUP_ORIENT_CENTER);
	setContent(createGenlist(getEvasObject()));
}

Evas_Object *RadioPopup::createGenlist(Evas_Object *parent)
{
	m_Genlist = elm_genlist_add(parent);
	elm_genlist_homogeneous_set(m_Genlist, EINA_TRUE);
	elm_genlist_mode_set(m_Genlist, ELM_LIST_COMPRESS);
	elm_scroller_content_min_limit(m_Genlist, EINA_FALSE, EINA_TRUE);

	m_ItemClass = createItemClass();

	m_RadioGroup = elm_radio_add(m_Genlist);

	elm_radio_state_value_set(m_RadioGroup, -1);
	evas_object_smart_data_set(m_Genlist, this);

	return m_Genlist;
}

Elm_Genlist_Item_Class *RadioPopup::createItemClass()
{
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	RETVM_IF(!itc, NULL, "elm_genlist_item_class_new() failed");
	itc->item_style = "type1";
	itc->func.text_get = getItemText;
	itc->func.content_get = getItemContent;
	itc->func.del = delItem;
	return itc;
}

void RadioPopup::onSelect(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	RadioPopup *popup = static_cast<RadioPopup *>(data);
	RadioItemData *itemData = static_cast<RadioItemData *>(elm_object_item_data_get(item));

	elm_radio_value_set(popup->m_RadioGroup, itemData->value);

	if (popup->m_Callback) {
		popup->m_Callback(itemData->value);
	}

	elm_genlist_item_selected_set(item, EINA_FALSE);
}

char *RadioPopup::getItemText(void *data, Evas_Object *obj, const char *part)
{
	RadioItemData *itemData = static_cast<RadioItemData *>(data);

	if (strcmp(part, "elm.text") == 0) {
		return strdup(_(itemData->text.c_str()));
	}
	return nullptr;
}

Evas_Object *RadioPopup::getItemContent(void *data, Evas_Object *obj, const char *part)
{
	RadioItemData *itemData = static_cast<RadioItemData *>(data);
	RadioPopup *popup = static_cast<RadioPopup *>(evas_object_smart_data_get(obj));

	if (strcmp(part, "elm.swallow.end") == 0) {
		Evas_Object *radio = elm_radio_add(obj);

		elm_radio_group_add(radio, popup->m_RadioGroup);
		elm_radio_state_value_set(radio, itemData->value);
		return radio;
	}
	return nullptr;
}

void RadioPopup::delItem(void *data, Evas_Object *obj)
{
	RadioItemData *itemData = static_cast<RadioItemData *>(data);
	delete itemData;
}
