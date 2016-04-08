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
#include "Contacts/Model/ContactRecordData.h"
#include "Common/Strings.h"

using namespace Contacts::Model;

SearchData::SearchData(ContactData &contactData, int matchPattern)
	: ContactData(contactData.getType()),
	  m_ContactData(contactData),
	  m_MatchPattern(matchPattern)
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

int SearchData::compare(const char *str, int pattern)
{
	m_MatchPattern = m_ContactData.compare(str, pattern);
	return m_MatchPattern;
}

const SearchData::Strings &SearchData::getMatchedStrings(MatchPattern pattern, const char *str)
{
	if (pattern & m_MatchPattern) {
		switch (pattern) {
			case MatchName:
				if (m_Names.empty()) {
					initNames(str);
				}
				return m_Names;
			case MatchNumber:
				if (m_Numbers.empty()) {
					initNumbers(str);
				}
				break;
			default:
				break;
		}
	}

	return m_Numbers;
}


void SearchData::initNames(const char *str) const
{
	if (getType() == TypeContact) {
		addName(static_cast<ContactRecordData &>(m_ContactData).getAlphaName().c_str(), str);
	} else {
		addName(getName(), str);
	}
}

void SearchData::initNumbers(const char *str) const
{
	if (getType() == TypeContact || getType() == TypePerson) {
		for (auto &&number : static_cast<ContactRecordData &>(m_ContactData).getNumbers()) {
			addNumber(number, str);
		}
	} else {
		addNumber(getNumber(), str);
	}
}

void SearchData::addName(const char *name, const char *str) const
{
	const char *position = strstr(name, str);
	m_Names.push_back(Common::highlightString(name, position - name, strlen(str)));
}

void SearchData::addNumber(const char *number, const char *str) const
{
	const char *position = strstr(number, str);
	m_Numbers.push_back(Common::highlightString(number, position - number, strlen(str)));
}
