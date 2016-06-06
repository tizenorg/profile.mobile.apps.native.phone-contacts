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
using namespace std::placeholders;

SearchEngine::SearchEngine(SearchProvider &provider)
	: m_LastFoundIndex(-1),
	  m_SearchProvider(provider)
{
	m_SearchProvider.setSearchNotifyCallback(std::bind(&SearchEngine::onProviderNotify, this, _1, _2));
}

void SearchEngine::search(std::string query)
{
	if (query.empty()) {
		clear();
		if (!m_Query.empty()) {
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

		if (m_Query.empty() || nothingFound) {
			incrementalSearch(m_SearchProvider.getDataList(), query);
		} else if (matchPos == m_History.size()) {
			updateSearchResult(m_History.back());
		} else {
			incrementalSearch(m_History[matchPos], query);
		}
	}

	m_Query = std::move(query);
}

bool SearchEngine::empty() const
{
	return m_History.empty() || m_History.back().empty();
}

bool SearchEngine::needSearch(const std::string &query)
{
	if (query.size() >= m_Query.size()
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
	for (auto &&data : m_SearchProvider.getDataList()) {
		SearchData *searchData = static_cast<SearchData *>(data);
		searchData->setSearchResult(nullptr);
	}
}

SearchEngine::SearchHistory::iterator SearchEngine::getMatch(const std::string &query)
{
	size_t minSize = std::min(m_Query.size(), query.size());
	auto itPair = std::mismatch(m_Query.begin(), m_Query.begin() + minSize, query.begin());

	return skipEmptyResults(itPair.first - m_Query.begin());
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

void SearchEngine::onProviderNotify(contacts_changed_e changed, SearchData *searchData)
{
	if (changed == CONTACTS_CHANGE_INSERTED) {
		onInserted(searchData);
	}
}

void SearchEngine::onInserted(SearchData *searchData)
{
	SearchResult *lastResult = nullptr;
	std::string query;
	query.reserve(m_Query.size());

	for (size_t i = 0; i < m_History.size(); ++i) {
		if (i < (size_t)m_LastFoundIndex && m_History[i].empty()) {
			continue;
		}

		query.append(1, m_Query[i]);
		SearchResultPtr searchResult = searchData->compare(query);

		if (searchResult) {
			m_History[i].emplace_back(searchData, std::move(searchResult));
			if (i == m_History.size() - 1) {
				lastResult = m_History[i].back().second.get();
			}
		}
	}
	searchData->setSearchResult(lastResult);
}
