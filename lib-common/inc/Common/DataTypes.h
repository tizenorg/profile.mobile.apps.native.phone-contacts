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

#ifndef COMMON_DATA_TYPES_H
#define COMMON_DATA_TYPES_H

namespace Common
{
	/**
	 * @brief Change event types.
	 */
	enum ChangeType
	{
		ChangeInsert = 1 << 0,
		ChangeUpdate = 1 << 1,
		ChangeDelete = 1 << 2
	};

	/**
	 * @brief Designates Contacts API model enumeration types.
	 */
	enum EnumType
	{
		EnumNumberType,       /**< contacts_number_type_e */
		EnumEmailType,        /**< contacts_email_type_e */
		EnumAddressType,      /**< contacts_address_type_e */
		EnumEventType,        /**< contacts_event_type_e */
		EnumRelationshipType, /**< contacts_relationship_type_e */
		EnumUrlType,          /**< contacts_url_type_e */
		EnumMessengerType,    /**< contacts_messenger_type_e */
		EnumLogType           /**< contacts_phone_log_type_e */
	};
}

#endif /* COMMON_DATA_TYPES_H */
