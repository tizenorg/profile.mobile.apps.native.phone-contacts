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

#ifndef CONTACTS_DB_PROVIDER_H
#define CONTACTS_DB_PROVIDER_H

#include "Contacts/DbChangeObserver.h"
#include "Contacts/ContactDataProvider.h"

#include <tizen.h>

namespace Contacts
{
	/**
	 * @brief Provider class, that handles ContactData insertion from DB
	 */
	class EXPORT_API DbProvider : public ContactDataProvider
	{
	public:
		DbProvider();
		virtual ~DbProvider() override;

	protected:
		/**
		 * @param[in]   id      ContactData ID
		 * @return New ContactData
		 */
		virtual ContactDataPtr createContactData(int id) = 0;

	private:
		DbChangeObserver::CallbackHandle m_Handle;
	};
}

#endif /* CONTACTS_DB_PROVIDER_H */
