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

#include "Phone/Dialer/SearchResult.h"
#include "Phone/Dialer/SearchUtils.h"

using namespace Phone::Dialer;
using namespace Phone::Dialer::Utils;

SearchResult::SearchResult(ResultType type, const contacts_record_h record)
	: m_Type(type), m_Id(0), m_SpeedDialId(0)
{
	if (record) {
		if (!fillWithRecord(type, record)) {
			m_Type = ResultNone;
		}
	}
}

std::string SearchResult::getSearchString() const
{
	switch (m_Type) {
		case ResultSpeedDial:
			return std::string(1, (m_SpeedDialId + '0'));
		case ResultLog:
		case ResultNumber:
			return m_Number;
		case ResultName:
			return m_MaskedName;
		case ResultNone:
		case ResultMax:
		default:
			return "";
	}
}

ResultType SearchResult::getType() const
{
	return m_Type;
}

int SearchResult::getId() const
{
	return m_Id;
}

int SearchResult::getSpeedDialId() const
{
	return m_SpeedDialId;
}

const std::string &SearchResult::getName(bool isHighlighted) const
{
	bool returnHighlight = isHighlighted && (m_Type == ResultName) && !m_HighlightedText.empty();
	return returnHighlight ? m_HighlightedText : m_Name;
}

const std::string &SearchResult::getNumber(bool isHighlighted) const
{
	bool returnHighlight = isHighlighted && ((m_Type == ResultLog) || (m_Type == ResultNumber)) && !m_HighlightedText.empty();
	return returnHighlight ? m_HighlightedText : m_Number;
}

const std::string &SearchResult::getHighlightedText() const
{
	return m_HighlightedText;
}

bool SearchResult::updateHighlightText(const std::string searchStr, size_t position)
{
	switch(m_Type)
	{
		case ResultLog:
		case ResultNumber:
			m_HighlightedText = Utils::highlightTextByPos(m_Number, position, searchStr.size());
			return true;
		case ResultName:
			m_HighlightedText = Utils::highlightTextByPos(m_Name, position, searchStr.size());
			return true;
		case ResultNone:
		case ResultSpeedDial:
		case ResultMax:
		default:
			return false;
	}
}

bool SearchResult::fillWithRecord(ResultType type, const contacts_record_h record)
{
	switch(type)
	{
		case ResultSpeedDial:
			fillSpeedDial(record);
			return true;
		case ResultLog:
			fillLog(record);
			return true;
		case ResultName:
		case ResultNumber:
			fillContact(type, record);
			return true;
		case ResultNone:
		case ResultMax:
		default:
			return false;
	}
}

void SearchResult::fillSpeedDial(const contacts_record_h record)
{
	char *tempStr = NULL;

	contacts_record_get_int(record, _contacts_speeddial.person_id, &m_Id);
	contacts_record_get_int(record, _contacts_speeddial.speeddial_number, &m_SpeedDialId);
	contacts_record_get_str_p(record, _contacts_speeddial.number, &tempStr);
	m_Number = tempStr;
	contacts_record_get_str_p(record, _contacts_speeddial.display_name, &tempStr);
	m_Name = tempStr;
}

void SearchResult::fillLog(const contacts_record_h record)
{
	char *tempStr = NULL;

	contacts_record_get_int(record, _contacts_phone_log.person_id, &m_Id);
	contacts_record_get_str_p(record, _contacts_phone_log.address, &tempStr);
	m_Number = tempStr;
}

void SearchResult::fillContact(ResultType type, const contacts_record_h record)
{
	char *tempStr = NULL;

	contacts_record_get_int(record, _contacts_contact_number.contact_id, &m_Id);
	contacts_record_get_str_p(record, _contacts_contact_number.display_name, &tempStr);
	m_Name = tempStr;
	contacts_record_get_str_p(record, _contacts_contact_number.number, &tempStr);
	m_Number = tempStr;
	if(type == ResultName) {
		m_MaskedName = Utils::contactNameToMask(m_Name);
	}
}
