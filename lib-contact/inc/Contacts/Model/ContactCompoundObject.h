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

#ifndef CONTACTS_MODEL_CONTACT_COMPOUND_OBJECT_H
#define CONTACTS_MODEL_CONTACT_COMPOUND_OBJECT_H

#include "Contacts/Model/ContactObject.h"
#include "Utils/Range.h"

#include <string>
#include <vector>

namespace Contacts
{
	namespace Model
	{
		/**
		 * @brief Interface of an object whose several fields can be represented
		 *        as a single text string.
		 */
		class ContactCompoundObject : public ContactObject
		{
		public:
			using ContactObject::ContactObject;

			/**
			 * @see ContactField::isEmpty()
			 */
			virtual bool isEmpty() const override;

			/**
			 * @return Compound field value.
			 */
			virtual std::string getValue() const = 0;

			/**
			 * @brief Set compound field value.
			 */
			virtual void setValue(std::string value) = 0;

		protected:
			/**
			 * @brief Get merged values of specified fields.
			 * @param[in]   fieldIds  IDs of field to get values from
			 * @return Merged values.
			 *
			 * @see ContactCompoundObject::mergeStrings()
			 */
			std::string getValues(Utils::Range<const unsigned *> fieldIds) const;

			/**
			 * @brief Set values of text fields with corresponding indexes.
			 * @param[in]   values  Iterators for array of pointers to values
			 */
			void setValues(Utils::Range<const char **> values);

			/**
			 * @brief Split string using space delimiter.
			 * @details Input string is modified by inserting terminator characters.
			 * @param[in]   string  String to split
			 * @return Split strings.
			 */
			static std::vector<const char *> splitString(char *string);

			/**
			 * @brief Merge several strings into one using space delimiter.
			 * @param[in]   strings Strings to merge
			 * @return Merged string.
			 */
			static std::string mergeStrings(const std::vector<const char *> &strings);
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_COMPOUND_OBJECT_H */
