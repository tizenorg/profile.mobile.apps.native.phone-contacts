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
#include "Contacts/Model/SearchProvider.h"
#include "Contacts/Utils.h"

#include <utility>
#include <algorithm>

using namespace Contacts::Model;

SearchEngine::SearchEngine(SearchProvider &searchProvider)
	: m_LastFoundIndex(-1),
	  m_SearchProvider(searchProvider)
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

const ContactDataList *SearchEngine::getSearchResult() const
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

void SearchEngine::firstSearch(const std::string &query)
{
	clear();

	const ContactDataList searchList = m_SearchProvider.getContactDataList();

	if (!searchList.empty()) {
		m_Cache.resize(query.size());
		m_Cache.front() = std::move(searchList);
		m_LastFoundIndex = 0;

		if (query.size() > 1) {
			if (!searchInCache(m_Cache.begin(), query)) {
				clear();
			}
		}
	}
}

void SearchEngine::chooseSearch(const std::string &query)
{
	if (m_Query.empty()) {
		firstSearch(query);
	}

	if (!needSearch(query)) {
		return;
	}

	m_Cache.resize(query.size());
	auto rIt = firstMismatch(query);
	if (rIt == m_Cache.rend()) {//Perform initial search
		firstSearch(query);
	} else {
		searchInCache(rIt.base() - 1, query);
	}
}

bool SearchEngine::searchInCache(SearchHistory::iterator from, const std::string &query)
{
	ContactDataList searchRes;
	for (auto &&contactData : *from) {
		//Todo: Compare
	}

	if (!searchRes.empty()) {
		m_LastFoundIndex = m_Cache.size() - 1;
		m_Cache.back() = std::move(searchRes);
		return true;
	} else {
		return false;
	}
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

bool SearchEngine::needSearch(const std::string &query)
{
	if (query.size() >= m_Query.size()
		&&(int)(m_Cache.size() - 1) > m_LastFoundIndex) {
		return false;
	}
	return true;
}
