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

#include "Contacts/List/PersonSearchItem.h"

#include "Common/Utils.h"

using namespace Contacts::List;
using namespace Contacts::Model;
using namespace Contacts::List::Model;
using Ux::SelectResult;
using Common::highlightStr;

PersonSearchItem::PersonSearchItem(Model::PersonSearchData &searchData)
	: PersonItem(searchData.getPerson()), m_SearchData(searchData)
{
	m_SearchData.setChangeCallback(std::bind(&PersonSearchItem::onSearchDataChanged, this));
}

char *PersonSearchItem::getText(Evas_Object *parent, const char *part)
{
	const SearchResult *searchResult = m_SearchData.getSearchResult();
	if (searchResult) {
		SearchResult::MatchedField matchedField = searchResult->getMatchedField();

		if (matchedField == SearchResult::MatchedName) {
			if (strcmp(part, PART_CONTACT_NAME) == 0) {
				return strdup(getHighlightedStr().c_str());
			}
		} else if (matchedField == SearchResult::MatchedNumber) {
			if (strcmp(part, PART_SUBTEXT) == 0) {
				return strdup(getHighlightedStr().c_str());
			}
		}

		if (matchedField != SearchResult::MatchedNone) {
			if (strcmp(part, PART_SUBTEXT) == 0) {
				return Utils::safeDup(m_SearchData.getNumber());
			}
		}
	}

	return PersonItem::getText(parent, part);
}

Eina_Bool PersonSearchItem::compare(Evas_Object *parent, void *filter)
{
	const char *str = (const char *) filter;
	bool isSearching = str && *str;

	if (isSearching) {
		bool isEqual = (bool) m_SearchData.getSearchResult();
		setExcluded(!isEqual);
		return isEqual;
	}

	return true;
}

std::string PersonSearchItem::getHighlightedStr() const
{
	const SearchResult *searchResult = m_SearchData.getSearchResult();
	return searchResult
			? highlightStr(searchResult->getMatchedString(), searchResult->getMatchedSubstring()) : "";
}

void PersonSearchItem::onSearchDataChanged()
{
	elm_genlist_item_fields_update(getObjectItem(), PART_CONTACT_NAME, ELM_GENLIST_ITEM_FIELD_TEXT);
	elm_genlist_item_fields_update(getObjectItem(), PART_SUBTEXT, ELM_GENLIST_ITEM_FIELD_TEXT);
}
