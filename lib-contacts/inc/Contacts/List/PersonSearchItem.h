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

#ifndef CONTACTS_LIST_PERSON_SEARCH_ITEM_H
#define CONTACTS_LIST_PERSON_SEARCH_ITEM_H

#include "Ux/SelectItem.h"
#include "Contacts/List/Model/PersonSearchData.h"

#include <functional>

#define PART_PERSON_NAME        "elm.text"
#define PART_PERSON_SUBSTR      "elm.text.sub"
#define PART_PERSON_THUMBNAIL   "elm.swallow.icon"
#define PART_PERSON_CHECK       "elm.swallow.end"

namespace Contacts
{
	namespace List
	{
		class PersonSearchItem : public Ux::SelectItem
		{
		public:
			explicit PersonSearchItem(Model::PersonSearchData &person);

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;
			virtual Ux::SelectResult getDefaultResult() const override;

			char *getStr(std::function<const char *()> getter,
					Model::PersonSearchData::MatchedField matchedField) const;
			char *getSubstr() const;

			Model::PersonSearchData &m_SearchData;
		};
	}
}

#endif /* CONTACTS_LIST_PERSON_SEARCH_ITEM_H */
