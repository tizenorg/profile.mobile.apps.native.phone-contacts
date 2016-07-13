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
		SearchResultPtr result = getComparator(subField)(str);
		if (result) {
			return result;
		}
	}

	return nullptr;
}

SearchResultPtr PersonSearchData::compareName(const std::string &str)
{
	const char *name = getName();
	while (name && *name) {
		const char *delim = strchr(name, ' ');

		if (strncasecmp(name, str.c_str(), str.size()) == 0) {
			return SearchResultPtr(new SearchResult(SearchResult::MatchedName, getName(), { name, str.size() }));
		}

		if (delim) {
			name = delim + 1;
		} else {
			return nullptr;
		}
	}

	return nullptr;
}

SearchResultPtr PersonSearchData::compareNickname(const std::string &str)
{
	return caseCompare(getPerson().getNickname(), str, SearchResult::MatchedNickname);
}

SearchResultPtr PersonSearchData::compareNote(const std::string &str)
{
	return caseCompare(getPerson().getNote(), str, SearchResult::MatchedNote);
}

SearchResultPtr PersonSearchData::compareOrganization(const std::string &str)
{
	contacts_record_h record = getPerson().getOrganization();
	auto nameResult = caseCompare(getRecordStr(record, _contacts_company.name), str, SearchResult::MatchedOrganization);

	return nameResult ? std::move(nameResult) :
		std::move(caseCompare(getRecordStr(record, _contacts_company.job_title), str, SearchResult::MatchedOrganization));
}

SearchResultPtr PersonSearchData::compareAddress(const std::string &str)
{
	return fieldsCompare(getPerson().getAddresses(), _contacts_address.street, str, SearchResult::MatchedAddress);
}

SearchResultPtr PersonSearchData::compareEmail(const std::string &str)
{
	return fieldsCompare(getPerson().getEmails(), _contacts_email.email, str, SearchResult::MatchedEmail);
}

SearchResultPtr PersonSearchData::compareNumber(const std::string &str)
{
	for (auto &&numberRange : getPerson().getNumbers()) {
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


const PersonSearchData::Comparator &PersonSearchData::getComparator(size_t index)
{
	static Comparator compMethods[] = {
		/* PersonName         = */ std::bind(&PersonSearchData::compareName, this, _1),
		/* PersonNickname     = */ std::bind(&PersonSearchData::compareNickname, this, _1),
		/* PersonOrganization = */ std::bind(&PersonSearchData::compareOrganization, this, _1),
		/* PersonAddress      = */ std::bind(&PersonSearchData::compareAddress, this, _1),
		/* PersonEmail        = */ std::bind(&PersonSearchData::compareEmail, this, _1),
		/* PersonNote         = */ std::bind(&PersonSearchData::compareNote, this, _1),
		/* PersonNumber       = */ std::bind(&PersonSearchData::compareNumber, this, _1),
	};

	return compMethods[index];
}
