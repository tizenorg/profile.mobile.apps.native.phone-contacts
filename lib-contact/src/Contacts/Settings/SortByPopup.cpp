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

#include "Contacts/Settings/SortByPopup.h"
#include "Utils/Logger.h"
#include <app_i18n.h>
#include <contacts_setting.h>

using namespace Contacts::Settings;

SortByPopup::SortByPopup()
	: m_RadioGroup(nullptr)
{
}

void SortByPopup::onCreated()
{
	elm_popup_orient_set(getEvasObject(), ELM_POPUP_ORIENT_CENTER);
	setTitle("IDS_PB_MBODY_SORT_BY");
	setContent(createGenlist(getEvasObject()));
}

Evas_Object *SortByPopup::createGenlist(Evas_Object *parent)
{
	Evas_Object *genlist = elm_genlist_add(parent);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_scroller_content_min_limit(genlist, EINA_FALSE, EINA_TRUE);

	Elm_Genlist_Item_Class *itc = createItemClass();

	m_RadioGroup = elm_radio_add(genlist);
	contacts_name_sorting_order_e type = CONTACTS_NAME_SORTING_ORDER_FIRSTLAST;
	contacts_setting_get_name_sorting_order(&type);
	elm_radio_state_value_set(m_RadioGroup, type);
	evas_object_smart_data_set(genlist, this);

	elm_genlist_item_append(genlist, itc, (void *) CONTACTS_NAME_SORTING_ORDER_FIRSTLAST, NULL,
			ELM_GENLIST_ITEM_NONE, onSelect, this);
	elm_genlist_item_append(genlist, itc, (void *) CONTACTS_NAME_SORTING_ORDER_LASTFIRST, NULL,
			ELM_GENLIST_ITEM_NONE, onSelect, this);

	elm_genlist_item_class_free(itc);
	return genlist;
}

void SortByPopup::onSelect(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	int type = (int) elm_object_item_data_get(item);
	SortByPopup *popup = static_cast<SortByPopup *>(data);
	elm_radio_value_set(popup->m_RadioGroup, type);

	contacts_setting_set_name_sorting_order((contacts_name_sorting_order_e) type);

	elm_genlist_item_selected_set(item, EINA_FALSE);
	elm_genlist_item_update(item);
}

char *SortByPopup::getItemText(void *data, Evas_Object *obj, const char *part)
{
	int type = (int) data;
	if (strcmp(part, "elm.text") == 0) {
		if (CONTACTS_NAME_SORTING_ORDER_FIRSTLAST == type) {
			return strdup(_("IDS_PB_OPT_FIRST_NAME"));
		} else if (CONTACTS_NAME_SORTING_ORDER_LASTFIRST == type) {
			return strdup(_("IDS_PB_OPT_LAST_NAME"));
		}
	}
	return nullptr;
}

Elm_Genlist_Item_Class *SortByPopup::createItemClass()
{
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	RETVM_IF(!itc, NULL, "elm_genlist_item_class_new() failed");
	itc->item_style = "type1";
	itc->func.text_get = getItemText;
	itc->func.content_get = getItemContent;
	return itc;
}

Evas_Object *SortByPopup::getItemContent(void *data, Evas_Object *obj, const char *part)
{
	SortByPopup *popup = static_cast<SortByPopup *>(evas_object_smart_data_get(obj));

	if (strcmp(part, "elm.swallow.end") == 0) {
		int type = (int) data;
		Evas_Object *radio = elm_radio_add(obj);

		elm_radio_group_add(radio, popup->m_RadioGroup);
		elm_radio_state_value_set(radio, type);

		return radio;
	}
	return nullptr;
}
