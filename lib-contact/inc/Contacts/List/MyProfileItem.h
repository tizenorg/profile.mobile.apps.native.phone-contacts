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
#include <functional>

#define PART_MY_PROFILE_NAME        "elm.text"
#define PART_MY_PROFILE_THUMBNAIL   "elm.swallow.icon"

namespace Contacts
{
	namespace List
	{
		class MyProfileItem : public Ui::GenlistItem
		{
		public:
			/**
			 * @brief Item select callback
			 */
			typedef std::function<void()> SelectedCallback;

			/**
			 * @brief Create My profile genlist item
			 * @param[in]   myProfile   My profile object
			 */
			MyProfileItem(Model::MyProfilePtr myProfile);

			/**
			 * @return my profile object
			 */
			const Model::MyProfile &getMyProfile() const;

			/**
			 * @brief Set my profile object to item
			 * @param[in]   myProfile   My profile object
			 */
			void setMyProfile(Model::MyProfilePtr myProfile);

			/**
			 * @brief Set item selected callback
			 * @param[in]   callback    Callback, that will be invoked on item selection
			 */
			void setSelectedCallback(SelectedCallback callback);

		protected:
			virtual void onSelected() override;

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			Model::MyProfilePtr m_MyProfile;
			SelectedCallback m_OnSelected;
		};
	}
}

#endif /* CONTACTS_LIST_MY_PROFILE_ITEM_H */
