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

#include "Phone/Dialer/SearchResultsPopup.h"
#include "Phone/Dialer/SearchUtils.h"
#include "Phone/Dialer/KeypadEntry.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include "DialerLayout.h"

#include <app_i18n.h>

using namespace Phone::Dialer;

SearchResultsPopup::SearchResultsPopup(const SearchResults *results)
	: m_Results(results)
{
}

void SearchResultsPopup::setSelectedCallback(SelectedCallback callback)
{
	m_OnSelected = std::move(callback);
}

void SearchResultsPopup::onCreated()
{
	Evas_Object *popup = getEvasObject();
	elm_object_style_set(popup, "theme_bg");
	elm_popup_orient_set(popup, ELM_POPUP_ORIENT_CENTER);

	const size_t bufferSize = 32;
	char buffer[bufferSize];
	snprintf(buffer, bufferSize, _("IDS_KPD_HEADER_SEARCH_RESULTS_HPD_ABB"), m_Results->size());

	setTitle(buffer);
	setContent(createContactList(popup));
}

Evas_Object *SearchResultsPopup::createContactList(Evas_Object *parent)
{
	Evas_Object *genlist = elm_genlist_add(parent);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_scroller_content_min_limit(genlist, EINA_FALSE, EINA_TRUE);
	evas_object_smart_callback_add(genlist, "selected",
			makeCallback(&SearchResultsPopup::onItemSelected), this);

	Elm_Genlist_Item_Class *itc = createItemClass();

	for (auto &&info : *m_Results) {
		elm_genlist_item_append(genlist, itc, &info, nullptr,
				ELM_GENLIST_ITEM_NONE, nullptr, nullptr);
	}

	elm_genlist_item_class_free(itc);
	return genlist;
}

Elm_Genlist_Item_Class *SearchResultsPopup::createItemClass()
{
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	RETVM_IF(!itc, NULL, "elm_genlist_item_class_new() failed");
	itc->item_style = "type1";
	itc->func.text_get = getItemText;
	itc->func.content_get = getItemContent;
	return itc;
}

char *SearchResultsPopup::getItemText(void *data, Evas_Object *obj, const char *part)
{
	SearchResultPtr info = *(SearchResultPtr *)data;
	const std::string &name = info->getName(true);
	const std::string &number = info->getNumber(true);

	if (!strcmp(part, "elm.text")) {
		return strdup(name.empty() ? number.c_str() : name.c_str());
	} else if (!strcmp(part, "elm.text.sub")) {
		if (!name.empty()) {
			return strdup(number.c_str());
		}
	}

	return nullptr;
}

Evas_Object *SearchResultsPopup::getItemContent(void *data, Evas_Object *obj, const char *part)
{
	SearchResultPtr info = *(SearchResultPtr *)data;
	if (!strcmp(part, "elm.swallow.icon")) {
		return Utils::createThumbnail(obj, info->getId());
	}

	return nullptr;
}

void SearchResultsPopup::onItemSelected(Evas_Object *obj, void *event_info)
{
	elm_genlist_item_selected_set((Elm_Object_Item *)event_info, EINA_FALSE);

	SearchResultPtr info = *(SearchResultPtr *)elm_object_item_data_get((Elm_Object_Item *)event_info);
	if (m_OnSelected) {
		m_OnSelected(info);
	}

	delete this;
}
