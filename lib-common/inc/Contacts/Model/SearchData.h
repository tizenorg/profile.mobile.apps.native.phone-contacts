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

namespace Contacts
{
	namespace Model
	{
		class SearchData : public ContactData
		{
		public:
			SearchData(ContactData &contactData, Type type);

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
			 * @see ContactData::getType()
			 */
			virtual Type getType() const override;

			/**
			 * @see ContactData::compare
			 */
			virtual bool compare(const char *str) override;

		private:
			friend class SearchProvider;

			ContactData &m_ContactData;
		};
	}
}

#endif /* CONTACTS_MODEL_SEARCH_DATA_H */
