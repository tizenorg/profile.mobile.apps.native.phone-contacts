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

#ifndef CONTACTS_LIST_MODEL_CONTACT_NUMBER_DATA_H
#define CONTACTS_LIST_MODEL_CONTACT_NUMBER_DATA_H

#include "Contacts/DbChangeObserver.h"
#include "Contacts/List/Model/ContactRecordData.h"

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			class ContactNumberData : public ContactRecordData
			{
			public:
				/**
				 * @brief Create contact object
				 * @param[in]   record  Contact record
				 */
				ContactNumberData(contacts_record_h record);
				~ContactNumberData();

				/**
				 * @see ContactData::compare()
				 */
				virtual bool compare(const char *str) override;

			private:
				friend class ContactNumberProvider;

				void setChangedCallback(DbChangeObserver::Callback callback);
				void unsetChangedCallback();

				void onChanged(int id, contacts_changed_e changeType);

				DbChangeObserver::CallbackHandle m_Handle;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_CONTACT_NUMBER_DATA_H */
