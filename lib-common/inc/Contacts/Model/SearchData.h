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
#include "Utils/Range.h"
#include <tizen.h>

namespace Contacts
{
	namespace Model
	{
		class EXPORT_API SearchData : public ContactData
		{
		public:
			/**
			 * @brief Determines range of chars, which are as substring
			 */
			typedef Utils::Range<const char *> Substring;

			/**
			 * @brief Determines which field conform to searchable string
			 * @see compare()
			 */
			enum MatchedField
			{
				MatchedNone,  /**< Not matched */
				MatchedName,  /**< Matched by name */
				MatchedNumber /**< Matched by number */
			};

			/**
			 * @brief Create SearchData object
			 * @param[in]   contactData     Contact reference
			 */
			SearchData(ContactData &contactData);

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
			 * @brief Compare @a str with SearchData field
			 *
			 * Compare searchable @a str string with one of
			 * name/number SearchData field. Create found substring if found one.
			 *
			 * @param[in]   str     Searchable string
			 * @return true if found something, false if not
			 */
			virtual bool compare(const char *str) = 0;

			/**
			 * @return Matched field
			 * @see MatchedField
			 */
			MatchedField getMatchedField() const;

			/**
			 * @return Matched substring
			 */
			const Substring &getSubstring() const;

		protected:
			/**
			 * @brief Set new MatchedField pattern
			 * @see MatchedField
			 */
			void setMatchedField(MatchedField field);

			/**
			 * @brief Set substring, found by @ref compare() method
			 * @see Substring
			 */
			void setSubstring(Substring substring);

			/**
			 * @return ContactData reference
			 */
			ContactData &getContactData();

		private:
			ContactData &m_ContactData;
			Substring m_Substring;
			MatchedField m_MatchedField;
		};
	}
}

#endif /* CONTACTS_MODEL_SEARCH_DATA_H */
