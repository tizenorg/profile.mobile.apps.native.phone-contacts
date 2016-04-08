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

#ifndef PHONE_DIALER_SEARCH_UTILS_H
#define PHONE_DIALER_SEARCH_UTILS_H

#include <string>
#include <contacts.h>
#include <Evas.h>

namespace Phone
{
	namespace Dialer
	{
		namespace Utils
		{
			/**
			 * @param[in]	number	Speed dial number
			 * @return contact with speed dial matched @a number
			 */
			contacts_list_h getSpeedDial(char number);

			/**
			 * @param[in]	digit	Digit in log number
			 * @return log list with numbers contains @a digit
			 */
			contacts_list_h getLogList(char digit);

			/**
			 * @param[in]	digit	Digit on keypad
			 * @return contacts list with names contains @a digit on phone keypad
			 */
			contacts_list_h getContactListByName(char digit);

			/**
			 * @param[in]	digit	Digit in contact's number
			 * @return contacts list with numbers contains @a digit
			 */
			contacts_list_h getContactListByNumber(char digit);

			/**
			 * @param[in]	name	Contact name
			 * @return converted contact name to letter mask
			 */
			std::string contactNameToMask(const std::string &name);

			/**
			 * param[in]	parent				Parent layout
			 * param[in]	contactId			Contact ID
			 */
			Evas_Object *createThumbnail(Evas_Object *parent, int contactId);
		}
	}
}

#endif /* PHONE_DIALER_SEARCH_UTILS_H */
