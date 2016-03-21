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

#ifndef CONTACTS_OBJECT_TYPE_H
#define CONTACTS_OBJECT_TYPE_H

		/**
		 * @brief Specifies additional interface supported by object
		 */
		enum ContactObjectInterface
		{
			ObjectTyped = 0x100, /**< Object supports ContactTypedObject interface */
			ObjectCompound = 0x100 << 1, /**< Object supports ContactCompoundObject interface */
		};

		/**
		 * @brief Subtype for field of TypeObject type
		 */
		enum ContactObjectType
		{
			ObjectTypeContact      = 1,
			ObjectTypeMyProfile    = 2,
			ObjectTypeImage        = 3,
			ObjectTypeName         = 4 + ObjectCompound,
			ObjectTypeNumber       = 5 + ObjectTyped,
			ObjectTypeEmail        = 6 + ObjectTyped,
			ObjectTypeNote         = 7,
			ObjectTypeAddress      = 8 + ObjectTyped,
			ObjectTypeEvent        = 9 + ObjectTyped,
			ObjectTypeRelationship = 10 + ObjectTyped,
			ObjectTypeUrl          = 11 + ObjectTyped,
			ObjectTypeMessenger    = 12 + ObjectTyped,
			ObjectTypeNickname     = 13,
			ObjectTypeCompany      = 14,
			ObjectTypePhoneticName = 15 + ObjectCompound
		};




#endif /* CONTACTS_OBJECT_TYPE_H */
