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
		if (!needSearch(query)) {
			return;
		}

		m_History.resize(query.size());

		if (m_Query.empty()) {
			initialSearch(query);
		} else {
			auto it = firstMismatch(query);
			if (it == m_History.end()) {//Perform initial search
				clear();
				initialSearch(query);
			} else {
				incrementalSearch(*it, query);
			}
		}
	}

	m_Query = query;
}

const SearchEngine::DataList *SearchEngine::getSearchResult() const
{
	if (!m_History.empty()) {
		return &m_History.back();
	}

	return nullptr;
}

bool SearchEngine::empty() const
{
	return m_History.empty() || m_History.back().empty();
}

void SearchEngine::setDataList(DataList *dataList)
{
	m_DataList = dataList;
}

bool SearchEngine::needSearch(const std::string &query)
{
	if (query.size() >= m_Query.size()
		&&(int)(m_History.size() - 1) > m_LastFoundIndex) {
		return false;
	}
	return true;
}

void SearchEngine::initialSearch(const std::string &query)
{
	RETM_IF(!m_DataList, "Initial data list is not assigned");
	incrementalSearch(*m_DataList, query);
}

void SearchEngine::incrementalSearch(const DataList &list, const std::string &query)
{
	DataList searchRes = search(list, query);

	if (!searchRes.empty()) {
		m_LastFoundIndex = m_History.size() - 1;
		m_History.back() = std::move(searchRes);
	}
}

SearchEngine::DataList SearchEngine::search(const DataList &list, const std::string &query)
{
	DataList searchRes;
	for (auto &&data : list) {
		auto searchData = static_cast<SearchData *>(data);
		if (searchData->compare(query)) {
			searchRes.push_back(searchData);
		}
	}

	return searchRes;
}

SearchEngine::SearchHistory::iterator SearchEngine::firstMismatch(const std::string &query)
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
