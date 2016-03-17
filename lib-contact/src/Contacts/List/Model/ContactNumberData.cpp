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

#include "Contacts/List/Model/ContactNumberData.h"
#include <cstring>

using namespace Contacts::List::Model;

ContactNumberData::ContactNumberData(contacts_record_h record)
	: ContactRecordData(TypeContact, record)
{
}

bool ContactNumberData::compare(const char *str)
{
	const char *number = getNumber();
	return number && str && strstr(number, str);
}
