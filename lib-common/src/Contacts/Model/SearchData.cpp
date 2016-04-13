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
#include <cstring>

using namespace Contacts::Model;

SearchData::SearchData(ContactData &contactData)
	: ContactData(contactData.getType()), m_ContactData(contactData)
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

SearchData::Substring SearchData::getSubstring(const char *str1, const char *str2)
{
	Substring substring;
	const char *res = strstr(str1, str2);

	if (res) {
		substring = { res, res + strlen(str2) };
	}

	return substring;
}
