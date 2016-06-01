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

#include "Contacts/List/Model/PersonSearchData.h"
#include "Contacts/List/Model/Person.h"
#include "Contacts/Model/ContactNumberData.h"
#include <cstring>

using namespace Contacts::Model;
using namespace Contacts::List::Model;

PersonSearchData::PersonSearchData(Person &person)
	: SearchData(person)
{
}

Person &PersonSearchData::getPerson()
{
	return static_cast<Person &>(getContactData());
}

const char *PersonSearchData::getNumber() const
{
	const SearchResult *searchResult = getSearchResult();
	if (searchResult) {
		if (searchResult->getMatchedField() == SearchResult::MatchedNumber && searchResult->getMatchedString()) {
			return searchResult->getMatchedString();
		}
	}

	return SearchData::getNumber();
}

SearchResultPtr PersonSearchData::compare(const std::string &str)
{
	if (str.empty()) {
		return SearchResultPtr(new SearchResult());
	}

	SearchResultPtr nameResult = compareName(str);
	return nameResult ? std::move(nameResult) : std::move(compareNumber(str));
}

SearchResultPtr PersonSearchData::compareName(const std::string &str)
{
	const char *name = getName();
	while (name && *name) {
		const char *delim = strchr(name, ' ');
		if (!delim) {
			delim = name + strlen(name);
		}

		if (strncasecmp(name, str.c_str(), str.size()) == 0) {
			return SearchResultPtr(new SearchResult(SearchResult::MatchedName, getName(), { name, str.size() }));
		}

		if (*delim) {
			name = delim + 1;
		} else {
			return nullptr;
		}
	}

	return nullptr;
}

SearchResultPtr PersonSearchData::compareNumber(const std::string &str)
{
	auto &person = static_cast<Person &>(getContactData());
	for (auto &&number : person.getNumbers()) {
		const char *pos = strstr(number->getNumber(), str.c_str());
		if (pos) {
			return SearchResultPtr(new SearchResult(SearchResult::MatchedNumber, number->getNumber(), { pos, str.size() }));
		}
	}

	return nullptr;
}
