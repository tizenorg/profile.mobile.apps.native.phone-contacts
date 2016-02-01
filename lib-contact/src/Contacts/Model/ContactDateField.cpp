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

#include "Contacts/Model/ContactDateField.h"

#define YEAR_BASE       1900
#define YEAR_FACTOR     10000
#define MONTH_BASE      1
#define MONTH_FACTOR    100

using namespace Contacts::Model;

void ContactDateField::reset()
{
	time_t now = time(nullptr);
	setValue(*localtime(&now));
}

bool ContactDateField::isChanged() const
{
	tm value = getValue();
	return difftime(mktime(&value), m_InitialValue) != 0;
}

tm ContactDateField::getValue() const
{
	return getValue(getRecord());
}

void ContactDateField::setValue(tm date)
{
	contacts_record_set_int(getRecord(), getPropertyId(), convertDate(date));
}

tm ContactDateField::getValue(contacts_record_h record) const
{
	int date = 0;
	contacts_record_get_int(record, getPropertyId(), &date);
	return convertDate(date);
}

void ContactDateField::onInitialize(contacts_record_h record)
{
	tm value = getValue(record);
	m_InitialValue = mktime(&value);
}

tm ContactDateField::convertDate(int value)
{
	struct tm date = { 0 };
	date.tm_year = value / YEAR_FACTOR - YEAR_BASE;
	date.tm_mon = (value % YEAR_FACTOR) / MONTH_FACTOR - MONTH_BASE;
	date.tm_mday = value % YEAR_FACTOR % MONTH_FACTOR;

	return date;
}

int ContactDateField::convertDate(const tm &date)
{
	return (date.tm_year + YEAR_BASE) * YEAR_FACTOR
		 + (date.tm_mon + MONTH_BASE) * MONTH_FACTOR
		 +  date.tm_mday;
}
