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

#ifndef CONTACTS_MODEL_PERSON_SEARCH_DATA_H
#define CONTACTS_MODEL_PERSON_SEARCH_DATA_H

#include "Contacts/Model/SearchData.h"

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			class Person;

			class PersonSearchData : public Contacts::Model::SearchData
			{
			public:
				/**
				 * @brief Create PersonSearchData object
				 * @param[in]   person      Person reference
				 */
				PersonSearchData(Person &person);

				/**
				 * @see ContactData::getNumber()
				 */
				virtual const char *getNumber() const override;

				/**
				 * @see SearchData::compare()
				 */
				virtual bool compare(const std::string &str) override;

			private:
				friend class SearchProvider;
			};
		}
	}
}

#endif /* CONTACTS_MODEL_PERSON_SEARCH_DATA_H */
