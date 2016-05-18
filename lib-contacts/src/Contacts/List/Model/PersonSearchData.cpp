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

using namespace Contacts::List::Model;

PersonSearchData::PersonSearchData(Person &person)
	: SearchData(person)
{
}

const char *PersonSearchData::getNumber() const
{
	if (getMatchedField() == MatchedNumber && getMatchedString()) {
		return getMatchedString();
	}

	return SearchData::getNumber();
}

bool PersonSearchData::compare(const std::string &str)
{
	const char *pos = strstr(getName(), str.c_str());
	if (pos) {
		setMatchedField(MatchedName);
		setMatchedString(getName());
		setMatchedSubstring({ pos, str.size() });

		return true;
	}

	auto &person = static_cast<Person &>(getContactData());
	for (auto &&number : person.getNumbers()) {
		pos = strstr(number->getNumber(), str.c_str());
		if (pos) {
			setMatchedField(MatchedNumber);
			setMatchedString(number->getNumber());
			setMatchedSubstring({ pos, str.size() });

			return true;
		}
	}

	setMatchedField(MatchedNone);
	setMatchedString(nullptr);
	setMatchedSubstring({});
	return false;
}
