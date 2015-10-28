/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "PhDialerPredictiveNumber.h"
#include "PhDialerPredictiveNumberUtils.h"
#include "PhDialerSearchInfo.h"
#include "PhDialerPredictiveNumberTypes.h"
#include "ContactsUtils.h"
#include <utility>
#include <algorithm>

using namespace Ph::Dialer;

namespace
{
	typedef contacts_list_h (*ContactListGetter)(char digit);

	ContactListGetter contactListGetters[] = {
		Utils::getSpeedDial,
		Utils::getLogList,
		Utils::getContactListByName,
		Utils::getContactListByNumber
	};
}

PredictiveNumber::PredictiveNumber()
	:m_LastFoundIndex(-1)
{}

void PredictiveNumber::search(const std::string &number)
{
	if(number.empty()) {
		clear();
	} else {
		chooseSearch(number);
	}

	m_Number = number;
}

const SearchList * PredictiveNumber::getSearchResult() const
{
	if(!m_PredictiveNumberCache.empty()) {
		return &m_PredictiveNumberCache.back();
	}

	return nullptr;
}

bool Ph::Dialer::PredictiveNumber::empty() const
{
	return m_PredictiveNumberCache.empty() || m_PredictiveNumberCache.back().empty();
}

void PredictiveNumber::distinctLogs(SearchList &searchList)
{
	auto logPred = [](PSearchInfo pSearchInfo) -> bool {
		return pSearchInfo->getType() == IT_LOG;
	};

	auto logComp = [](PSearchInfo lhs, PSearchInfo rhs) -> bool {
		return lhs->getSearchString() < rhs->getSearchString();
	};

	auto eqPred = [](PSearchInfo lhs, PSearchInfo rhs) -> bool {
		return lhs->getSearchString() == rhs->getSearchString();
	};

	SearchList::iterator beg = std::find_if(searchList.begin(), searchList.end(), logPred);
	SearchList::iterator end = std::find_if_not(beg, searchList.end(), logPred);

	if(beg != end) {
		std::sort(beg, end, logComp);
		SearchList::iterator itForErase = std::unique(beg, end, eqPred);

		searchList.erase(itForErase, end);
	}
}

void PredictiveNumber::firstSearch(const std::string &number)
{
	clear();

	SearchList searchList = searchInDB(number);

	if(!searchList.empty()) {
		distinctLogs(searchList);

		m_PredictiveNumberCache.resize(number.size());
		m_PredictiveNumberCache.front() = std::move(searchList);
		m_LastFoundIndex = 0;

		if(number.size() > 1) {
			if(!searchInCache(m_PredictiveNumberCache.begin(), number)) {
				clear();
			}
		}
	}
}

SearchList Ph::Dialer::PredictiveNumber::searchInDB(const std::string &number)
{
	SearchList searchList;
	contacts_record_h record = NULL;

	for(int i = IT_SPEED_DIAL; i < IT_MAX; ++i) {
		contacts_list_h list = contactListGetters[i](number.front());
		CONTACTS_LIST_FOREACH(list, record) {
			SearchInfo searchInfo((InfoType)i, record);
			if(searchInfo.getType() != IT_NONE) {
				size_t position = searchInfo.getSearchString().find(number);
				if (position != std::string::npos) {
					searchInfo.updateHighlightText(number, position);
				}
				searchList.push_back(std::make_shared<SearchInfo>(std::move(searchInfo)));
			}
		}
		contacts_list_destroy(list, true);
	}

	return searchList;
}

void PredictiveNumber::chooseSearch(const std::string &number)
{
	if(m_Number.empty()) {
		firstSearch(number);
	}

	if(!needSearch(number)) {
		return;
	}

	m_PredictiveNumberCache.resize(number.size());
	auto rIt = firstMismatch(number);
	if(rIt == m_PredictiveNumberCache.rend()) {//Perform initial search
		firstSearch(number);
	} else {
		searchInCache(rIt.base() - 1, number);
	}
}

bool PredictiveNumber::searchInCache(Cache::iterator from, const std::string &number)
{
	SearchList searchRes;
	for(PSearchInfo &sInfo : *from) {
		if(sInfo) {
			size_t position = sInfo->getSearchString().find(number);
			if (position != std::string::npos) {
				sInfo->updateHighlightText(number, position);
				searchRes.push_back(sInfo);
			}
		}
	}

	if(!searchRes.empty()) {
		m_LastFoundIndex = m_PredictiveNumberCache.size() - 1;
		m_PredictiveNumberCache.back() = std::move(searchRes);
		return true;
	} else {
		return false;
	}
}

Cache::reverse_iterator PredictiveNumber::firstMismatch(const std::string &number)
{
	size_t minSize = std::min(m_Number.size(), number.size());
	auto itPair = std::mismatch(m_Number.begin(), m_Number.begin() + minSize, number.begin());

	auto rIt = skipEmptyResults(itPair.first - m_Number.begin());
	return rIt;
}

Cache::reverse_iterator PredictiveNumber::skipEmptyResults(size_t offset)
{
	auto rIt = std::reverse_iterator<Cache::iterator>(m_PredictiveNumberCache.begin() + offset);

	while(rIt != m_PredictiveNumberCache.rend() && rIt->empty()) {
		++rIt;
	}

	return rIt;
}

void PredictiveNumber::clear()
{
	m_PredictiveNumberCache.clear();
	m_LastFoundIndex = -1;
}

bool PredictiveNumber::needSearch(const std::string &number)
{
	if( number.size() >= m_Number.size()
		&&(int)(m_PredictiveNumberCache.size() - 1) > m_LastFoundIndex) {
		return false;
	}
	return true;
}

void PredictiveNumber::searchFromScratch(const std::string& number)
{
	if(number.empty()) {
		clear();
	} else {
		firstSearch(number);
	}

	m_Number = number;
}
