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

#ifndef CONTACTS_LIST_MY_PROFILE_GROUP_H
#define CONTACTS_LIST_MY_PROFILE_GROUP_H

#include "Ui/GenGroupItem.h"

namespace Contacts
{
	namespace List
	{
		/**
		 * @brief Represents the group containing "My Profile" genlist item.
		 */
		class MyProfileGroup : public Ui::GenGroupItem
		{
		public:
			MyProfileGroup();

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
		};
	}
}

#endif /* CONTACTS_LIST_MY_PROFILE_GROUP_H */
