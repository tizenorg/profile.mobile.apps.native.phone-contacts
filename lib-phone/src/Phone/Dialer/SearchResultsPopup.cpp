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
#include "Ui/Scale.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include "DialerLayout.h"

#include <app_i18n.h>

#define TITLE_BUFFER_SIZE 32
#define BOX_MAX_HEIGHT 900
#define GENLIST_ITEM_HEIGHT 144

using namespace Phone::Dialer;

SearchResultsPopup::SearchResultsPopup(const SearchResults *results)
	: m_Results(results)
{
}

Evas_Object *SearchResultsPopup::onCreate(Evas_Object *parent)
{
	Evas_Object *popup = Popup::onCreate(parent);

	Evas_Object *box = elm_box_add(popup);
	Evas_Object *genlist = ListPopup::createGenlist(box);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	int genlistHeight = m_Results->size() * GENLIST_ITEM_HEIGHT;
	if (genlistHeight > BOX_MAX_HEIGHT) {
		evas_object_size_hint_min_set(box, 0, Ui::getScaledValue(BOX_MAX_HEIGHT));
		elm_scroller_content_min_limit(genlist, EINA_FALSE, EINA_FALSE);
	}

	evas_object_show(genlist);
	elm_box_pack_end(box, genlist);
	elm_object_part_content_set(popup, "elm.swallow.content", box);

	return popup;
}

void SearchResultsPopup::onCreated()
{
	char buffer[TITLE_BUFFER_SIZE];
	snprintf(buffer, sizeof(buffer), _("IDS_KPD_HEADER_SEARCH_RESULTS_HPD_ABB"), m_Results->size());
	setTitle(buffer);

	for (auto &&result : *m_Results) {
		addItem((void *) &result);
	}
}

char *SearchResultsPopup::getItemText(void *data, const char *part)
{
	SearchResultPtr result = *(SearchResultPtr *) data;
	const std::string &name = result->getName(true);
	const std::string &number = result->getNumber(true);

	if (!strcmp(part, "elm.text")) {
		return strdup(name.empty() ? number.c_str() : name.c_str());
	} else if (!strcmp(part, "elm.text.sub")) {
		if (!name.empty()) {
			return strdup(number.c_str());
		}
	}

	return nullptr;
}

Evas_Object *SearchResultsPopup::getItemContent(void *data, const char *part)
{
	SearchResultPtr result = *(SearchResultPtr *) data;
	if (!strcmp(part, "elm.swallow.icon")) {
		return Utils::createThumbnail(getEvasObject(), result->getId());
	}

	return nullptr;
}
