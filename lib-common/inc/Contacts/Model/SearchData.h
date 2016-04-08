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

#ifndef CONTACTS_MODEL_SEARCH_DATA_H
#define CONTACTS_MODEL_SEARCH_DATA_H

#include "Contacts/Model/ContactData.h"
#include <string>
#include <vector>

namespace Contacts
{
	namespace Model
	{
		class SearchData : public ContactData
		{
		public:
			typedef std::vector<std::string> Strings;

			SearchData(ContactData &contactData, int matchPattern = MatchAll);

			/**
			 * @see ContactData::getId()
			 */
			virtual int getId() const override;

			/**
			 * @see ContactData::getName()
			 */
			virtual const char *getName() const override;

			/**
			 * @see ContactData::getNumber()
			 */
			virtual const char *getNumber() const override;

			/**
			 * @see ContactData::getImagePath
			 */
			virtual const char *getImagePath() const override;

			/**
			 * @see ContactData::compare
			 */
			virtual int compare(const char *str, int pattern) override;

			/**
			 * @brief Get list of matched strings, with highlighted matched substring from compare() method
			 * @param[in]   pattern     See @ref MatchPattern
			 * @param[in]   str         String that should be highlighted as substring in each returned string
			 */
			const Strings &getMatchedStrings(MatchPattern pattern, const char *str);

		private:
			friend class SearchProvider;

			void initNames(const char *str) const;
			void initNumbers(const char *str) const;
			void addName(const char *name, const char *str) const;
			void addNumber(const char *number, const char *str) const;

			ContactData &m_ContactData;

			int m_MatchPattern;
			mutable Strings m_Names;
			mutable Strings m_Numbers;
		};
	}
}

#endif /* CONTACTS_MODEL_SEARCH_DATA_H */
