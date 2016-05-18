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
#include "Ui/Thumbnail.h"

using namespace Contacts::List;
using namespace Contacts::Model;
using namespace Contacts::List::Model;
using Ux::SelectResult;
using Common::highlightStr;

PersonSearchItem::PersonSearchItem(Person& person)
	: PersonItem(person), m_SearchData(nullptr)
{
}

void PersonSearchItem::setSearchData(Model::PersonSearchData *searchData)
{
	m_SearchData = searchData;
}

char *PersonSearchItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_CONTACT_NAME) == 0) {
		return getStr(PersonSearchData::MatchedName, m_SearchData->getName());
	} else if (strcmp(part, PART_SUBSTR) == 0) {
		return getSubstr();
	}

	return nullptr;
}

char *PersonSearchItem::getStr(SearchData::MatchedField matchedField,
		const char *matchedStr) const
{
	if (matchedStr) {
		if (m_SearchData->getMatchedField() == matchedField) {
			return strdup(highlightStr(matchedStr, m_SearchData->getMatchedSubstring()).c_str());
		} else {
			return strdup(matchedStr);
		}
	}

	return nullptr;
}

char *PersonSearchItem::getSubstr() const
{
	SearchData::MatchedField matchedField = m_SearchData->getMatchedField();
	const char *matchedString = m_SearchData->getMatchedString();

	if (matchedField == SearchData::MatchedNumber) {
		return getStr(matchedField, matchedString);
	}

	return nullptr;
}
