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

#include "Phone/Dialer/SearchEngine.h"
#include "Phone/Dialer/SearchUtils.h"
#include "Common/Database/RecordUtils.h"
#include <utility>
#include <algorithm>

using namespace Phone::Dialer;
using namespace Phone::Dialer::Utils;

namespace
{
	typedef contacts_list_h (*ContactListGetter)(char digit);

	ContactListGetter contactListGetters[] = {
		getSpeedDial,
		getLogList,
		getContactListByName,
		getContactListByNumber
	};
}

SearchEngine::SearchEngine()
	: m_LastFoundIndex(-1)
{}

void SearchEngine::search(const std::string &number)
{
	if (number.empty()) {
		clear();
	} else {
		chooseSearch(number);
	}

	m_Number = number;
}

const SearchResults *SearchEngine::getSearchResult() const
{
	if (!m_Cache.empty()) {
		return &m_Cache.back();
	}

	return nullptr;
}

bool SearchEngine::empty() const
{
	return m_Cache.empty() || m_Cache.back().empty();
}

void SearchEngine::distinctLogs(SearchResults &searchList)
{
	auto logPred = [](SearchResultPtr pSearchInfo) {
		return pSearchInfo->getType() == ResultLog;
	};

	auto logComp = [](SearchResultPtr lhs, SearchResultPtr rhs) {
		return lhs->getSearchString() < rhs->getSearchString();
	};

	auto eqPred = [](SearchResultPtr lhs, SearchResultPtr rhs) {
		return lhs->getSearchString() == rhs->getSearchString();
	};

	SearchResults::iterator beg = std::find_if(searchList.begin(), searchList.end(), logPred);
	SearchResults::iterator end = std::find_if_not(beg, searchList.end(), logPred);

	if (beg != end) {
		std::sort(beg, end, logComp);
		SearchResults::iterator itForErase = std::unique(beg, end, eqPred);

		searchList.erase(itForErase, end);
	}
}

void SearchEngine::firstSearch(const std::string &number)
{
	clear();

	SearchResults searchList = searchInDB(number);

	if (!searchList.empty()) {
		distinctLogs(searchList);

		m_Cache.resize(number.size());
		m_Cache.front() = std::move(searchList);
		m_LastFoundIndex = 0;

		if (number.size() > 1) {
			if (!searchInCache(m_Cache.begin(), number)) {
				clear();
			}
		}
	}
}

SearchResults SearchEngine::searchInDB(const std::string &number)
{
	SearchResults searchList;
	contacts_record_h record = NULL;

	for (int i = ResultSpeedDial; i < ResultMax; ++i) {
		contacts_list_h list = contactListGetters[i](number.front());
		CONTACTS_LIST_FOREACH(list, record) {
			SearchResult searchInfo((ResultType)i, record);
			if (searchInfo.getType() != ResultNone) {
				size_t position = searchInfo.getSearchString().find(number);
				if (position != std::string::npos) {
					searchInfo.updateHighlightText(number, position);
				}
				searchList.push_back(std::make_shared<SearchResult>(std::move(searchInfo)));
			}
		}
		contacts_list_destroy(list, true);
	}

	return searchList;
}

void SearchEngine::chooseSearch(const std::string &number)
{
	if (m_Number.empty()) {
		firstSearch(number);
	}

	if (!needSearch(number)) {
		return;
	}

	m_Cache.resize(number.size());
	auto rIt = firstMismatch(number);
	if (rIt == m_Cache.rend()) {//Perform initial search
		firstSearch(number);
	} else {
		searchInCache(rIt.base() - 1, number);
	}
}

bool SearchEngine::searchInCache(SearchHistory::iterator from, const std::string &number)
{
	SearchResults searchRes;
	for (SearchResultPtr &sInfo : *from) {
		if (sInfo) {
			size_t position = sInfo->getSearchString().find(number);
			if (position != std::string::npos) {
				sInfo->updateHighlightText(number, position);
				searchRes.push_back(sInfo);
			}
		}
	}

	if (!searchRes.empty()) {
		m_LastFoundIndex = m_Cache.size() - 1;
		m_Cache.back() = std::move(searchRes);
		return true;
	} else {
		return false;
	}
}

SearchHistory::reverse_iterator SearchEngine::firstMismatch(const std::string &number)
{
	size_t minSize = std::min(m_Number.size(), number.size());
	auto itPair = std::mismatch(m_Number.begin(), m_Number.begin() + minSize, number.begin());

	auto rIt = skipEmptyResults(itPair.first - m_Number.begin());
	return rIt;
}

SearchHistory::reverse_iterator SearchEngine::skipEmptyResults(size_t offset)
{
	auto rIt = std::reverse_iterator<SearchHistory::iterator>(m_Cache.begin() + offset);

	while (rIt != m_Cache.rend() && rIt->empty()) {
		++rIt;
	}

	return rIt;
}

void SearchEngine::clear()
{
	m_Cache.clear();
	m_LastFoundIndex = -1;
}

bool SearchEngine::needSearch(const std::string &number)
{
	if (number.size() >= m_Number.size()
		&&(int)(m_Cache.size() - 1) > m_LastFoundIndex) {
		return false;
	}
	return true;
}

void SearchEngine::searchFromScratch(const std::string &number)
{
	if (number.empty()) {
		clear();
	} else {
		firstSearch(number);
	}

	m_Number = number;
}
