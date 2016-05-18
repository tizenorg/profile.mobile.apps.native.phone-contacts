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

PersonSearchItem::PersonSearchItem(Person &person)
	: PersonItem(person), m_SearchData(nullptr)
{
}

void PersonSearchItem::setSearchData(Model::PersonSearchData *searchData)
{
	m_SearchData = searchData;
	elm_genlist_item_fields_update(getObjectItem(), PART_CONTACT_NAME, ELM_GENLIST_ITEM_FIELD_TEXT);
	elm_genlist_item_fields_update(getObjectItem(), PART_SUBTEXT, ELM_GENLIST_ITEM_FIELD_TEXT);
}

char *PersonSearchItem::getText(Evas_Object *parent, const char *part)
{
	SearchData::MatchedField matchedField = m_SearchData->getMatchedField();

	if (strcmp(part, PART_CONTACT_NAME) == 0) {
		return strdup(matchedField == SearchData::MatchedName ?
				getHighlightedStr().c_str() : m_SearchData->getName());
	} else if (strcmp(part, PART_SUBTEXT) == 0) {
		return strdup(matchedField == SearchData::MatchedNumber ?
				getHighlightedStr().c_str() : m_SearchData->getNumber());
	}

	return nullptr;
}

std::string PersonSearchItem::getHighlightedStr() const
{
	return highlightStr(m_SearchData->getMatchedString(), m_SearchData->getMatchedSubstring());
}
