/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Contacts/Model/SearchData.h"

using namespace Contacts::Model;
using namespace Common;

SearchData::SearchData(ContactData &contactData)
	: ContactData(contactData.getType()),
	  m_ContactData(contactData),
	  m_FoundSubField(nullptr), m_MatchedField(MatchedNone)
{
}

int SearchData::getId() const
{
	return m_ContactData.getId();
}

const char *SearchData::getName() const
{
	return m_ContactData.getName();
}

const char *SearchData::getNumber() const
{
	return m_ContactData.getNumber();
}

const char *SearchData::getImagePath() const
{
	return m_ContactData.getImagePath();
}

SearchData::MatchedField SearchData::getMatchedField() const
{
	return m_MatchedField;
}

ContactData *SearchData::getFoundSubField() const
{
	return m_FoundSubField;
}

const Substring &SearchData::getSubstring() const
{
	return m_Substring;
}

ContactData &SearchData::getContactData()
{
	return m_ContactData;
}

void SearchData::setMatchedField(MatchedField field)
{
	m_MatchedField = field;
}

void SearchData::setFoundSubField(ContactData *subField)
{
	m_FoundSubField = subField;
}

void SearchData::setSubstring(Substring substring)
{
	m_Substring = std::move(substring);
}
