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

#ifndef CONTACTS_LIST_MY_PROFILE_ITEM_H
#define CONTACTS_LIST_MY_PROFILE_ITEM_H

#include "Contacts/List/Model/MyProfile.h"
#include "Ui/GenlistItem.h"

namespace Contacts
{
	namespace List
	{
		class MyProfileItem : public Ui::GenlistItem
		{
		public:
			MyProfileItem();

			/**
			 * @return My profile object
			 */
			Model::MyProfile &getMyProfile();

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;
			virtual void onSelected() override;
			void onUpdated(int changes);

			Model::MyProfile m_MyProfile;
		};
	}
}

#endif /* CONTACTS_LIST_MY_PROFILE_ITEM_H */
