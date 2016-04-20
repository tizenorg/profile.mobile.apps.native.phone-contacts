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

#ifndef CONTACTS_MODEL_CONTACT_NUMBER_DATA_H
#define CONTACTS_MODEL_CONTACT_NUMBER_DATA_H

#include "Contacts/Model/ContactData.h"
#include <contacts.h>

namespace Contacts
{
	namespace Model
	{
		class EXPORT_API ContactNumberData : public ContactData
		{
		public:
			/**
			 * @brief Create Number object
			 * @param[in]   contact         Contact object
			 * @param[in]   numberRecord    _contacts_number record
			 */
			ContactNumberData(ContactData &contact, contacts_record_h numberRecord);
			virtual ~ContactNumberData() override;

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
			 * @see ContactData::getImagePath()
			 */
			virtual const char *getImagePath() const override;

			//Todo: Implement support update and delete logic

		private:
			ContactData &m_ContactData;
			contacts_record_h m_NumberRecord;
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_NUMBER_DATA_H */
