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

#include <contacts.h>
#include <memory>

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
			class MyProfile
			{
			public:
				MyProfile(const MyProfile &myProfile) = delete;
				MyProfile &operator=(const MyProfile &myProfile) = delete;

				MyProfile();
				~MyProfile();

				/**
				 * @brief Get my profile ID
				 */
				const int getId() const;

				/**
				 * @return My profile name
				 */
				const char *getName() const;

				/**
				 * @return My profile image path
				 */
				const char *getImagePath() const;

				/**
				 * @return _contacts_my_profile record
				 */
				const contacts_record_h getRecord() const;

			private:
				contacts_record_h m_MyProfileRecord;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_MY_PROFILE_H */
