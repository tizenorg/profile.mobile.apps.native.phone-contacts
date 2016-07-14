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
#include "Common/Database/RecordUtils.h"

#include <cstring>

using namespace Contacts::Model;
using namespace Contacts::List::Model;
using namespace Common::Database;
using namespace std::placeholders;

namespace
{
	enum PersonSubField
	{
		PersonName,
		PersonNickname,
		PersonOrganization,
		PersonAddress,
		PersonEmail,
		PersonNote,
		PersonNumber,
		PersonMax
	};
}

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

	for (size_t subField = PersonName; subField < PersonMax; ++subField) {
		SearchResultPtr result = getCompFunc(subField)(getPerson(), str);
		if (result) {
			return result;
		}
	}

	return nullptr;
}

SearchResultPtr PersonSearchData::compareName(const Person &person, const std::string &str)
{
	const char *name = person.getName();
	while (name && *name) {
		const char *delim = strchr(name, ' ');

		if (strncasecmp(name, str.c_str(), str.size()) == 0) {
			return SearchResultPtr(new SearchResult(SearchResult::MatchedName, person.getName(), { name, str.size() }));
		}

		if (delim) {
			name = delim + 1;
		} else {
			return nullptr;
		}
	}

	return nullptr;
}

SearchResultPtr PersonSearchData::compareNickname(const Person &person, const std::string &str)
{
	return caseCompare(person.getNickname(), str, SearchResult::MatchedNickname);
}

SearchResultPtr PersonSearchData::compareNote(const Person &person, const std::string &str)
{
	return caseCompare(person.getNote(), str, SearchResult::MatchedNote);
}

SearchResultPtr PersonSearchData::compareOrganization(const Person &person, const std::string &str)
{
	contacts_record_h record = person.getOrganization();
	auto nameResult = caseCompare(getRecordStr(record, _contacts_company.name), str, SearchResult::MatchedOrganization);

	return nameResult ? std::move(nameResult) :
		std::move(caseCompare(getRecordStr(record, _contacts_company.job_title), str, SearchResult::MatchedOrganization));
}

SearchResultPtr PersonSearchData::compareAddress(const Person &person, const std::string &str)
{
	return fieldsCompare(person.getAddresses(), _contacts_address.street, str, SearchResult::MatchedAddress);
}

SearchResultPtr PersonSearchData::compareEmail(const Person &person, const std::string &str)
{
	return fieldsCompare(person.getEmails(), _contacts_email.email, str, SearchResult::MatchedEmail);
}

SearchResultPtr PersonSearchData::compareNumber(const Person &person, const std::string &str)
{
	for (auto &&numberRange : person.getNumbers()) {
		for (auto &&numberRecord : numberRange) {
			const char *number = getRecordStr(numberRecord, _contacts_number.number);
			const char *pos = strstr(number, str.c_str());

			if (pos) {
				return SearchResultPtr(new SearchResult(SearchResult::MatchedNumber, number, { pos, str.size() }));
			}

		}
	}

	return nullptr;
}

SearchResultPtr PersonSearchData::caseCompare(const char *fieldValue, const std::string &str, SearchResult::MatchedField matchedField)
{
	if (fieldValue) {
		if (strncasecmp(fieldValue, str.c_str(), str.size()) == 0) {
			return SearchResultPtr(new SearchResult(matchedField, fieldValue, { fieldValue, str.size() }));
		}
	}

	return nullptr;
}

SearchResultPtr PersonSearchData::fieldsCompare(Person::ContactChildRecords records,
		unsigned propertyId, const std::string &str, SearchResult::MatchedField matchedField)
{
	for (auto &&range : records) {
		for (auto &&record : range) {
			SearchResultPtr result = caseCompare(getRecordStr(record, propertyId), str, matchedField);
			if (result) {
				return result;
			}
		}
	}

	return nullptr;
}

PersonSearchData::Comparator PersonSearchData::getCompFunc(size_t i)
{
	static Comparator comparators[] = {
		/* PersonName         = */ &PersonSearchData::compareName,
		/* PersonNickname     = */ &PersonSearchData::compareNickname,
		/* PersonOrganization = */ &PersonSearchData::compareOrganization,
		/* PersonAddress      = */ &PersonSearchData::compareAddress,
		/* PersonEmail        = */ &PersonSearchData::compareEmail,
		/* PersonNote         = */ &PersonSearchData::compareNote,
		/* PersonNumber       = */ &PersonSearchData::compareNumber,
	};

	return comparators[i];
}
