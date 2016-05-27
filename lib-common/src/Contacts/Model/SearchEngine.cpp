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

#include "Contacts/Model/SearchEngine.h"
#include "Contacts/Model/SearchData.h"

#include <utility>
#include <algorithm>

using namespace Contacts::Model;

SearchEngine::SearchEngine(DataList &dataList)
	: m_LastFoundIndex(-1),
	  m_DataList(dataList)
{
}

void SearchEngine::search(std::string query)
{
	if (query.empty()) {
		clear();
		if (!m_PrevQuery.empty()) {
			resetSearchResult();
		}
	} else {
		if (!needSearch(query)) {
			return;
		}

		auto matchIt = getMatch(query);
		size_t matchPos = matchIt - m_History.begin();
		bool nothingFound = matchIt == m_History.end();

		m_History.erase(nothingFound ? m_History.begin() : (matchIt + 1), m_History.end());
		m_History.resize(query.size());

		if (m_PrevQuery.empty() || nothingFound) {
			incrementalSearch(m_DataList, query);
		} else if (matchPos == m_History.size()) {
			updateSearchResult(m_History.back());
		} else {
			incrementalSearch(m_History[matchPos], query);
		}
	}

	m_PrevQuery = std::move(query);
}

bool SearchEngine::empty() const
{
	return m_History.empty() || m_History.back().empty();
}

bool SearchEngine::needSearch(const std::string &query)
{
	if (query.size() >= m_PrevQuery.size()
		&&(int)(m_History.size() - 1) > m_LastFoundIndex) {
		return false;
	}

	return true;
}

template <typename List>
void SearchEngine::incrementalSearch(const List &list, const std::string &query)
{
	ResultList resultList;
	for (auto &&data : list) {
		SearchData *searchData = getSearchData(data);
		SearchResultPtr searchResult = searchData->compare(query);
		searchData->setSearchResult(searchResult.get());

		if (searchResult) {
			resultList.emplace_back(searchData, std::move(searchResult));
		}
	}

	if (!resultList.empty()) {
		m_LastFoundIndex = m_History.size() - 1;
		m_History.back() = std::move(resultList);
	}
}

SearchData *SearchEngine::getSearchData(ContactData *contactData)
{
	return static_cast<SearchData *>(contactData);
}

SearchData *SearchEngine::getSearchData(const SearchResultItem &resultItem)
{
	return resultItem.first;
}

void SearchEngine::updateSearchResult(ResultList &list)
{
	for (auto &&resultItem : list) {
		resultItem.first->setSearchResult(resultItem.second.get());
	}
}

void SearchEngine::resetSearchResult()
{
	for (auto &&data : m_DataList) {
		SearchData *searchData = static_cast<SearchData *>(data);
		searchData->setSearchResult(nullptr);
	}
}

SearchEngine::SearchHistory::iterator SearchEngine::getMatch(const std::string &query)
{
	size_t minSize = std::min(m_PrevQuery.size(), query.size());
	auto itPair = std::mismatch(m_PrevQuery.begin(), m_PrevQuery.begin() + minSize, query.begin());

	return skipEmptyResults(itPair.first - m_PrevQuery.begin());
}

SearchEngine::SearchHistory::iterator SearchEngine::skipEmptyResults(size_t offset)
{
	auto rIt = std::reverse_iterator<SearchHistory::iterator>(m_History.begin() + offset);

	while (rIt != m_History.rend() && rIt->empty()) {
		++rIt;
	}

	return rIt == m_History.rend() ? m_History.end() : rIt.base() - 1;
}

void SearchEngine::clear()
{
	m_History.clear();
	m_LastFoundIndex = -1;
}
