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
#ifndef PHONE_UTILS_H
#define PHONE_UTILS_H

#include <string>

namespace Phone
{
	/**
	 * @brief Get phone number assigned to specific speed dial number.
	 * @param[in]   speedNumber Number on the dialer
	 * @return Speed dial number or empty string if not assigned.
	 */
	std::string getSpeedDialNumber(int speedNumber);

	/**
	 * @brief Add speed dial number.
	 * @param[in]   speedNumber Number on the dialer
	 * @param[in]   numberId    ID of number to assign to @a speedNumber
	 * @return true on success, false if number with this @a numberId already exists.
	 */
	bool addSpeedDialNumber(int speedNumber, int numberId);

	/**
	 * @return Last incoming or outgoing call number.
	 */
	std::string getLastCallNumber();
}

#endif /* PHONE_UTILS_H */
