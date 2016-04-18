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

#include "Contacts/List/Model/FavoriteProvider.h"
#include "Contacts/List/Model/Person.h"
#include "Contacts/Utils.h"

using namespace Contacts;
using namespace Contacts::Model;
using namespace Contacts::List::Model;

FavoriteProvider::FavoriteProvider(Mode mode, int filterType)
	: PersonProvider(filterType), m_Mode(mode)
{
}

FavoriteProvider::~FavoriteProvider()
{
}

ContactData *FavoriteProvider::createContactData(contacts_record_h record)
{
	Person *person = nullptr;
	if (m_Mode == ModeOnly) {
		//TODO FavPerson should be created here
		person = new Person(record);
	} else {
		person = new Person(record);
	}

	return person;
}

void FavoriteProvider::setAdditionalQueryOptions(contacts_query_h query)
{
	if (m_Mode == ModeOnly) {
		contacts_query_set_sort(query, _contacts_person.favorite_priority, true);
	}
}

contacts_filter_h FavoriteProvider::getAdditionalFilter()
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_person._uri, &filter);

	if (m_Mode == ModeOnly) {
		contacts_filter_add_bool(filter, _contacts_person.is_favorite, true);
	} else {
		contacts_filter_add_bool(filter, _contacts_person.is_favorite, false);
	}

	return filter;
}
