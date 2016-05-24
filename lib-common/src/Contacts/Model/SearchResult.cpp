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

#include "Contacts/Model/SearchResult.h"

using namespace Contacts::Model;
using Common::Substring;

SearchResult::SearchResult()
	: m_MatchedField(MatchedNone),
	  m_MatchedString(nullptr)
{
}

SearchResult::SearchResult(MatchedField field, const char *str, Common::Substring substr)
	: m_MatchedField(field),
	  m_MatchedString(str),
	  m_MatchedSubstring(substr)
{
}

SearchResult::MatchedField SearchResult::getMatchedField() const
{
	return m_MatchedField;
}

const char *SearchResult::getMatchedString() const
{
	return m_MatchedString;
}

const Substring &SearchResult::getMatchedSubstring() const
{
	return m_MatchedSubstring;
}
