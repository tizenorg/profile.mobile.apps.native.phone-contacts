/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

//#include <efl_assist.h>

#include "ContactsCommon.h"
#include "ContactsDebug.h"
#include "CtSettingRadioPopup.h"
#include "CtString.h"


CtSettingRadioPopup::__ItemData::__ItemData()
: index(0)
{

}

CtSettingRadioPopup::CtSettingRadioPopup(const std::vector<std::string>& label, int selectedIndex)
: __label(label)
, __selectedIndex(selectedIndex)
, __resultCb(NULL)
{

}
CtSettingRadioPopup::~CtSettingRadioPopup()
{

}

Evas_Object* CtSettingRadioPopup::onCreate( Evas_Object* parent, void* viewParam)
{
	WHIT();

	setContent([this](Evas_Object* parent)->Evas_Object* {
		return __createContent(parent);
	});

	return WPopup::onCreate(parent, viewParam);
}

void CtSettingRadioPopup::onDestroy()
{

}


void CtSettingRadioPopup::setOnResultCb( std::function<void (int)> resultCb )
{
	__resultCb = resultCb;
}

void CtSettingRadioPopup::__itemSelectedCb(void* data, Evas_Object* obj, void* event_info)
{
	CtSettingRadioPopup* popup = static_cast<CtSettingRadioPopup*>(data);
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	int index = elm_genlist_item_index_get( item ) - 1; // genlist item index is starting from 1!
	popup->__selectedIndex = index;
	if( popup->__resultCb)
		popup->__resultCb(index);
}


Evas_Object* CtSettingRadioPopup::__createContent(Evas_Object *parent)
{
	// genlist class
	Elm_Genlist_Item_Class* itc = elm_genlist_item_class_new();
	WPRET_VM(!itc, NULL, "elm_genlist_item_class_new() failed");
	itc->item_style = "type1";
	itc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		__ItemData* itemData = static_cast<__ItemData*>(data);
		if(!strcmp(part, "elm.text")) {
			return strdup(itemData->label.c_str());
		}

		return NULL;
	};

	itc->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		if(!strcmp(part, "elm.swallow.end")) {
			__ItemData* itemData = static_cast<__ItemData*>(data);
			Evas_Object* radio = elm_radio_add(obj);
			Evas_Object *radioGroup = (Evas_Object*)evas_object_data_get(obj, "radioGroup");

			elm_object_style_set(radio, "list");
			elm_radio_state_value_set(radio, itemData->index);
			elm_radio_group_add(radio, radioGroup);

			evas_object_size_hint_weight_set(radio, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(radio, EVAS_HINT_FILL, EVAS_HINT_FILL);
			evas_object_propagate_events_set(radio, EINA_TRUE);

			if (itemData->popup->__selectedIndex == itemData->index) {
				elm_radio_value_set(radio, itemData->index);
			}
			return radio;
		}

		return NULL;
	};
	itc->func.del = [](void* data, Evas_Object* obj)
	{
		__ItemData* itemData = static_cast<__ItemData*>(data);
		delete itemData;
	};

	Evas_Object* genlist = elm_genlist_add(parent);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	elm_genlist_mode_set( genlist, ELM_LIST_COMPRESS );
	elm_scroller_content_min_limit( genlist, EINA_FALSE, EINA_TRUE ); // Automatically pop-up height is calculated

	Evas_Object* radioGroup = elm_radio_add(genlist);
	elm_object_style_set(radioGroup, "list");
	elm_radio_state_value_set(radioGroup, -1);
	evas_object_data_set(genlist, "radioGroup", radioGroup);

	elm_object_content_set(parent, genlist);

	int index = 0;
	for (auto iter = __label.begin(); iter != __label.end(); ++iter)
	{
		__ItemData* itemData = new __ItemData();
		itemData->label = *iter;
		itemData->popup = this;
		itemData->index = index++;
		elm_genlist_item_append(genlist, itc, itemData, NULL, ELM_GENLIST_ITEM_NONE, __itemSelectedCb, this);
	}

	elm_genlist_item_class_free(itc);

	evas_object_show(genlist);

	return genlist;
}


