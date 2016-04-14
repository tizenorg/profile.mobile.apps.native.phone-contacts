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

#ifndef CONTACTS_LIST_MODEL_MY_PROFILE_H
#define CONTACTS_LIST_MODEL_MY_PROFILE_H

#include "Contacts/Model/ContactData.h"
#include <contacts.h>

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			class MyProfile;

			typedef std::unique_ptr<MyProfile> MyProfilePtr;

			/**
			 * @brief My profile object
			 */
			class MyProfile : public Contacts::Model::ContactData
			{
			public:
				MyProfile();
				virtual ~MyProfile() override;

				/**
				 * @return My profile ID
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

				/**
				 * @brief Update record from the database
				 */
				void update();

			private:
				static contacts_record_h fetchRecord();
				static void onChanged(const char *uri, void *data);
				static void onNameFormatChanged(contacts_name_display_order_e order, void *data);

				contacts_record_h m_Record;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_MY_PROFILE_H */
