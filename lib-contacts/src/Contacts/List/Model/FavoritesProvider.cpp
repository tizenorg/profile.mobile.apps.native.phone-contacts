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

#include "Contacts/List/Model/FavoritesProvider.h"
#include "Contacts/List/Model/Person.h"
#include "Utils/Range.h"

using namespace Contacts;
using namespace Contacts::Model;
using namespace Contacts::List::Model;

namespace
{
	unsigned projection[] = {
		_contacts_person.id,
		_contacts_person.display_name,
		_contacts_person.display_name_index,
		_contacts_person.display_contact_id,
		_contacts_person.image_thumbnail_path,
		_contacts_person.is_favorite,
		_contacts_person.favorite_priority
	};
}

FavoritesProvider::FavoritesProvider(Mode mode, int filterType)
	: PersonProvider(filterType), m_Mode(mode)
{
}

FavoritesProvider::~FavoritesProvider()
{
}

Person *FavoritesProvider::createPerson(contacts_record_h record)
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

contacts_filter_h FavoritesProvider::getFilter() const
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_person._uri, &filter);

	if (auto baseFilter = PersonProvider::getFilter()) {
		contacts_filter_add_filter(filter, baseFilter);
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
		contacts_filter_destroy(baseFilter);
	}

	contacts_filter_add_bool(filter, _contacts_person.is_favorite, m_Mode == ModeOnly);

	return filter;
}

contacts_query_h FavoritesProvider::getQuery() const
{
	contacts_query_h query = PersonProvider::getQuery();
	if (query && m_Mode == ModeOnly) {
		contacts_query_set_projection(query, projection, Utils::count(projection));
		contacts_query_set_sort(query, _contacts_person.favorite_priority, true);
	}
	return query;
}

