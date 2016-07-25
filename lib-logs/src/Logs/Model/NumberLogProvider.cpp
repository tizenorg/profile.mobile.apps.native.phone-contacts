/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Logs/Model/NumberLogProvider.h"

using namespace Logs::Model;

NumberLogProvider::NumberLogProvider(const char *number)
	: m_Number(number)
{
}

bool NumberLogProvider::shouldGroupLogs(Log &log, Log &prevLog)
{
	return compareDate(log.getTime(), prevLog.getTime());
}

bool NumberLogProvider::shouldExist(Log &log)
{
	if (!m_Number && log.getNumber()) {
		return false;
	}
	return true;
}

contacts_filter_h NumberLogProvider::getFilter()
{
	contacts_filter_h filter = LogProvider::getFilter();
	if (m_Number) {
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
		contacts_filter_add_str(filter, _contacts_phone_log.address, CONTACTS_MATCH_EXACTLY, m_Number);
	}

	return filter;
}
