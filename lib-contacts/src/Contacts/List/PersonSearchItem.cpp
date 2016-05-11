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

PersonSearchItem::PersonSearchItem(PersonSearchData& person)
	: m_SearchData(person)
{
}

char *PersonSearchItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_PERSON_NAME) == 0) {
		return getStr(std::bind(&PersonSearchData::getName, std::ref(m_SearchData)),
				PersonSearchData::MatchedName);
	} else if (strcmp(part, PART_PERSON_SUBSTR) == 0) {
		return getSubstr();
	}

	return nullptr;
}

Evas_Object *PersonSearchItem::getContent(Evas_Object *parent, const char *part)
{
	using Ui::Thumbnail;

	if (strcmp(part, PART_PERSON_THUMBNAIL) == 0) {
		Thumbnail *thumbnail = Thumbnail::create(parent, Thumbnail::SizeSmall,
				m_SearchData.getImagePath());
		thumbnail->setSizeHint(true);
		return thumbnail->getEvasObject();
	} else if (strcmp(part, PART_PERSON_CHECK) == 0) {
		return SelectItem::getContent(parent, part);
	}

	return nullptr;
}

SelectResult PersonSearchItem::getDefaultResult() const
{
	return { 0, &m_SearchData };
}

char *PersonSearchItem::getStr(std::function<const char *()> getter,
		PersonSearchData::MatchedField matchedField) const
{
	const char *str = getter();
	if (str) {
		if (m_SearchData.getMatchedField() == matchedField) {
			return strdup(highlightStr(str, m_SearchData.getSubstring()).c_str());
		} else {
			return strdup(str);
		}
	}

	return nullptr;
}

char *PersonSearchItem::getSubstr() const
{
	ContactData *subField = m_SearchData.getFoundSubField();
	if (subField && subField->getType() == ContactData::TypeContactNumber) {
		return getStr(std::bind(&ContactData::getNumber, subField),
				PersonSearchData::MatchedNumber);
	}

	return nullptr;
}
