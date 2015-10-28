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

#include "PhDialerSearchInfo.h"
#include "PhDialerPredictiveNumberUtils.h"
#include "ContactsCommon.h"

using namespace Ph::Dialer;
using namespace Ph::Dialer::Utils;

SearchInfo::SearchInfo(InfoType type, const contacts_record_h record)
	: m_Type(type), m_Id(0), m_SpeedDialId(0)
{
	if(record) {
		if(!fillWithRecord(type, record)) {
			m_Type = IT_NONE;
		}
	}
}

std::string SearchInfo::getSearchString() const
{
	switch(m_Type) {
		case IT_SPEED_DIAL:
			return std::string(1, (m_SpeedDialId + '0'));
		case IT_LOG:
		case IT_NUMBER:
			return m_Number;
		case IT_NAME:
			return m_MaskedName;
		case IT_NONE:
		case IT_MAX:
		default:
			return "";
	}
}

InfoType SearchInfo::getType() const
{
	return m_Type;
}

int SearchInfo::getId() const
{
	return m_Id;
}

int Ph::Dialer::SearchInfo::getSpeedDialId() const
{
	return m_SpeedDialId;
}

const std::string &Ph::Dialer::SearchInfo::getName(bool isHighlighted) const
{
	bool returnHighlight = isHighlighted && (m_Type == IT_NAME) && !m_HighlightedText.empty();
	return returnHighlight ? m_HighlightedText : m_Name;
}

const std::string &Ph::Dialer::SearchInfo::getNumber(bool isHighlighted) const
{
	bool returnHighlight = isHighlighted && ((m_Type == IT_LOG) || (m_Type == IT_NUMBER)) && !m_HighlightedText.empty();
	return returnHighlight ? m_HighlightedText : m_Number;
}

const std::string &Ph::Dialer::SearchInfo::getHighlightedText() const
{
	return m_HighlightedText;
}

bool SearchInfo::updateHighlightText(const std::string searchStr, size_t position)
{
	switch(m_Type)
	{
		case IT_LOG:
		case IT_NUMBER:
			m_HighlightedText = ContactsCommon::highlightTextByPos(m_Number, position, searchStr.size());
			return true;
		case IT_NAME:
			m_HighlightedText = ContactsCommon::highlightTextByPos(m_Name, position, searchStr.size());
			return true;
		case IT_NONE:
		case IT_SPEED_DIAL:
		case IT_MAX:
		default:
			return false;
	}
}

bool Ph::Dialer::SearchInfo::fillWithRecord(InfoType type, const contacts_record_h record)
{
	switch(type)
	{
		case IT_SPEED_DIAL:
			fillSpeedDial(record);
			return true;
		case IT_LOG:
			fillLog(record);
			return true;
		case IT_NAME:
		case IT_NUMBER:
			fillContact(type, record);
			return true;
		case IT_NONE:
		case IT_MAX:
		default:
			return false;
	}
}

void SearchInfo::fillSpeedDial(const contacts_record_h record)
{
	char *tempStr = NULL;

	contacts_record_get_int(record, _contacts_speeddial.person_id, &m_Id);
	contacts_record_get_int(record, _contacts_speeddial.speeddial_number, &m_SpeedDialId);
	contacts_record_get_str_p(record, _contacts_speeddial.number, &tempStr);
	m_Number = tempStr;
	contacts_record_get_str_p(record, _contacts_speeddial.display_name, &tempStr);
	m_Name = tempStr;
}

void SearchInfo::fillLog(const contacts_record_h record)
{
	char *tempStr = NULL;

	contacts_record_get_int(record, _contacts_phone_log.person_id, &m_Id);
	contacts_record_get_str_p(record, _contacts_phone_log.address, &tempStr);
	m_Number = tempStr;
}

void SearchInfo::fillContact(InfoType type, const contacts_record_h record)
{
	char *tempStr = NULL;

	contacts_record_get_int(record, _contacts_contact_number.contact_id, &m_Id);
	contacts_record_get_str_p(record, _contacts_contact_number.display_name, &tempStr);
	m_Name = tempStr;
	contacts_record_get_str_p(record, _contacts_contact_number.number, &tempStr);
	m_Number = tempStr;
	if(type == IT_NAME) {
		m_MaskedName = Utils::contactNameToMask(m_Name);
	}
}
