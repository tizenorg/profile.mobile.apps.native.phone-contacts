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

#include "PhDialerResultListPopup.h"
#include "PhDialerLayout.h"
#include "PhDialerPredictiveNumberUtils.h"
#include "PhDialerEntry.h"
#include "ContactsCommon.h"
#include "ContactsDebug.h"
#include "ContactsLocalization.h"

#include <Elementary.h>

#define STYLE_ITEM		"type1"
#define PART_MAIN_TEXT	"elm.text"
#define PART_SUB_TEXT	"elm.text.sub"
#define PART_CONTENT	"elm.swallow.icon"

using namespace Ph::Dialer;

PhDialerResultListPopup::PhDialerResultListPopup(const Ph::Dialer::SearchList &result, PhDialerEntry &entry)
	: m_Result(result), m_Entry(entry)
{}

Evas_Object* PhDialerResultListPopup::onCreate(Evas_Object *parent, void *param)
{
	const size_t bufferSize = 32;
	char buffer[bufferSize];
	snprintf(buffer, bufferSize, T_("IDS_KPD_HEADER_SEARCH_RESULTS_HPD_ABB"), m_Result.size());

	setTitle(buffer);
	setContent(std::bind(&PhDialerResultListPopup::createContactList, this, std::placeholders::_1));
	setOrient(ELM_POPUP_ORIENT_CENTER);

	Evas_Object *popup = WPopup::onCreate(parent, param);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	elm_object_style_set(popup, "theme_bg");

	return popup;
}

Evas_Object* PhDialerResultListPopup::createContactList(Evas_Object *parent)
{
	Evas_Object *genlist = elm_genlist_add(parent);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	elm_genlist_mode_set( genlist, ELM_LIST_COMPRESS );

	Elm_Genlist_Item_Class *itc = createItemClass();

	for(auto &info : m_Result) {
		elm_genlist_item_append(genlist, itc, info.get(), nullptr,
				ELM_GENLIST_ITEM_NONE, onItemClicked, this);
	}

	elm_genlist_item_class_free(itc);
	return genlist;
}

Elm_Genlist_Item_Class* PhDialerResultListPopup::createItemClass()
{
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	WPRET_VM(!itc, NULL, "elm_genlist_item_class_new() failed");
	itc->item_style = STYLE_ITEM;
	itc->func.text_get = getItemText;
	itc->func.content_get = [](void *data, Evas_Object *obj, const char *part) -> Evas_Object* {
		const SearchInfo &info = *(SearchInfo*)data;

		if(!strcmp(part, PART_CONTENT)) {
			return Utils::createThumbnail(obj, info.getId());
		}

		return nullptr;
	};

	return itc;
}

char *PhDialerResultListPopup::getItemText(void *data, Evas_Object *obj, const char *part)
{
	const SearchInfo &info = *(SearchInfo*)data;
	const std::string &name = info.getName(true);
	const std::string &number = info.getNumber(true);

	if(!strcmp(part, PART_MAIN_TEXT)) {
		return strdup(name.empty() ? number.c_str() : name.c_str());
	} else if(!strcmp(part, PART_SUB_TEXT)) {
		if (!name.empty()) {
			return strdup(number.c_str());
		}
	}

	return nullptr;
}

void PhDialerResultListPopup::onItemClicked(void* data, Evas_Object* obj, void* event_info)
{
	elm_genlist_item_selected_set((Elm_Object_Item*)event_info, false);

	const SearchInfo *info = (SearchInfo*)elm_object_item_data_get((Elm_Object_Item*)event_info);

	PhDialerResultListPopup *self = (PhDialerResultListPopup*)data;
	self->m_Entry.setNumber(info->getNumber(false));

	self->destroy();
}
