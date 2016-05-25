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

void SearchEngine::search(const std::string &query)
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

		m_History.resize(query.size());

		if (m_PrevQuery.empty()) {                      //Perform initial search
			incrementalSearch(m_DataList, query);
		} else {
			auto it = firstMismatch(query);
			if (it == m_History.end()) {                //Perform initial search
				clear();
				incrementalSearch(m_DataList, query);
			} else {
				if (it == m_History.end() - 1) {        //No need to search, just rolling back to existed results
					updateSearchResult(*it);
				} else {
					incrementalSearch(*it, query);
				}
			}
		}
	}

	m_PrevQuery = query;
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
	SearchList searchRes;
	for (auto &&data : list) {
		SearchData *searchData = getSearchData(data);
		SearchResultPtr searchResult = searchData->compare(query);

		if (searchResult) {
			SearchListItem *item = new SearchListItem(searchData, std::move(searchResult));
			searchRes.push_back(SearchListItemPtr(item));
			searchData->setSearchResult(item->second.get());
		} else {
			searchData->setSearchResult(nullptr);
		}
	}

	if (!searchRes.empty()) {
		m_LastFoundIndex = m_History.size() - 1;
		m_History.back() = std::move(searchRes);
	}
}

SearchData *SearchEngine::getSearchData(ContactData *contactData)
{
	return static_cast<SearchData *>(contactData);
}

SearchData *SearchEngine::getSearchData(const SearchListItemPtr &searchItem)
{
	return searchItem->first;
}

void SearchEngine::updateSearchResult(SearchList &list)
{
	for (auto &&searchItem : list) {
		searchItem->first->setSearchResult(searchItem->second.get());
	}
}

void SearchEngine::resetSearchResult()
{
	for (auto &&data : m_DataList) {
		SearchData *searchData = static_cast<SearchData *>(data);
		searchData->setSearchResult(nullptr);
	}
}

SearchEngine::SearchHistory::iterator SearchEngine::firstMismatch(const std::string &query)
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
