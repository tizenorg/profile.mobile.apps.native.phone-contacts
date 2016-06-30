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
#include <phone_number.h>

using namespace Logs::Model;

NumberLogProvider::NumberLogProvider(std::string number)
	: m_Number(std::move(number))
{
}

bool NumberLogProvider::shouldGroupLogs(Log &log, Log &prevLog)
{
	return compareDate(log.getTime(), prevLog.getTime());
}

contacts_filter_h NumberLogProvider::getFilter()
{
	contacts_filter_h filter = LogProvider::getFilter();

	char *number = nullptr;
	phone_number_get_normalized_number(m_Number.c_str(), &number);

	contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	contacts_filter_add_str(filter, _contacts_phone_log.normalized_address, CONTACTS_MATCH_FULLSTRING, number);
	free(number);

	return filter;
}
