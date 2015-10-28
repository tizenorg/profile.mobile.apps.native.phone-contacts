/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef _ICT_CHANGE_LISTENER_H_
#define _ICT_CHANGE_LISTENER_H_

/**
 * @brief Interface for Change Listener
 */
class ICtChangeListener
{
public:
	virtual ~ICtChangeListener(){};
	/**
	 * @brief Calls on db changed
	 * @param[in]   viewUri The view URI of records whose changes are monitored
	 */
	virtual void onDbChanged(const char* viewUri) = 0;

	/**
	 * @brief Calls on complete get contacts
	 * @param[in]   offset The index from which gets results
	 */
	virtual void onCompleteGetContacts(int offset) = 0;

	/**
	 * @brief Calls on condition changed
	 */
	virtual void onConditionChagned() = 0;
};

#endif // _CT_CHANGE_LISTENER_H_

