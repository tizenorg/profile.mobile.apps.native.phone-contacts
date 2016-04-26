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
#include "Utils/Logger.h"

#include <utility>
#include <algorithm>

using namespace Contacts::Model;

SearchEngine::SearchEngine()
	: m_LastFoundIndex(-1),
	  m_DataList(nullptr)
{}

void SearchEngine::search(const std::string &query)
{
	if (query.empty()) {
		clear();
	} else {
		chooseSearch(query);
	}

	m_Query = query;
}

const SearchEngine::DataList *SearchEngine::getSearchResult() const
{
	if (!m_SearchHistory.empty()) {
		return &m_SearchHistory.back();
	}

	return nullptr;
}

bool SearchEngine::empty() const
{
	return m_SearchHistory.empty() || m_SearchHistory.back().empty();
}

void SearchEngine::chooseSearch(const std::string &query)
{
	if (m_Query.empty()) {
		firstSearch(query);
	}

	if (!needSearch(query)) {
		return;
	}

	m_SearchHistory.resize(query.size());
	auto rIt = firstMismatch(query);
	if (rIt == m_SearchHistory.rend()) {//Perform initial search
		firstSearch(query);
	} else {
		incrementalSearch(rIt.base() - 1, query);
	}
}

bool SearchEngine::needSearch(const std::string &query)
{
	if (query.size() >= m_Query.size()
		&&(int)(m_SearchHistory.size() - 1) > m_LastFoundIndex) {
		return false;
	}
	return true;
}

void SearchEngine::firstSearch(const std::string &query)
{
	RETM_IF(!m_DataList, "Initial data list is not assigned");

	clear();

	auto searchList = filter(*m_DataList, query);
	if (!searchList.empty()) {
		m_SearchHistory.resize(query.size());
		m_SearchHistory.front() = std::move(searchList);
		m_LastFoundIndex = 0;

		if (query.size() > 1) {
			if (!incrementalSearch(m_SearchHistory.begin(), query)) {
				clear();
			}
		}
	}
}

bool SearchEngine::incrementalSearch(SearchHistory::iterator from, const std::string &query)
{
	DataList searchRes = filter(*from, query);

	if (!searchRes.empty()) {
		m_LastFoundIndex = m_SearchHistory.size() - 1;
		m_SearchHistory.back() = std::move(searchRes);
		return true;
	} else {
		return false;
	}
}

SearchEngine::DataList SearchEngine::filter(const DataList &list, const std::string &query)
{
	DataList searchRes;
	for (auto &&data : list) {
		auto searchData = static_cast<SearchData *>(data);
		if (searchData->compare(query.c_str())) {
			searchRes.push_back(searchData);
		}
	}

	return searchRes;
}

SearchEngine::SearchHistory::reverse_iterator SearchEngine::firstMismatch(const std::string &query)
{
	size_t minSize = std::min(m_Query.size(), query.size());
	auto itPair = std::mismatch(m_Query.begin(), m_Query.begin() + minSize, query.begin());

	auto rIt = skipEmptyResults(itPair.first - m_Query.begin());
	return rIt;
}

SearchEngine::SearchHistory::reverse_iterator SearchEngine::skipEmptyResults(size_t offset)
{
	auto rIt = std::reverse_iterator<SearchHistory::iterator>(m_SearchHistory.begin() + offset);

	while (rIt != m_SearchHistory.rend() && rIt->empty()) {
		++rIt;
	}

	return rIt;
}

void SearchEngine::clear()
{
	m_SearchHistory.clear();
	m_LastFoundIndex = -1;
}
