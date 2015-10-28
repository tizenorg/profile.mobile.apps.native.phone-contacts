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

#ifndef _PH_DIALER_ENTRY_H_
#define _PH_DIALER_ENTRY_H_

#include "WControl.h"
#include <string>
#include <functional>

/**
 * @brief Phone dialer entry widget.
 */
class PhDialerEntry : public WControl
{
public:
	/**
	 * @return Entered number
	 */
	std::string getNumber();

	/**
	 * @brief Set number
	 */
	void setNumber(const std::string &number);

	/**
	 * @brief Insert character at the cursor position.
	 * @param   c   Character to insert
	 */
	void insert(char c);

	/**
	 * @brief Remove last character from the current entry.
	 */
	void popBack();

	/**
	 * @brief Erase current entry.
	 */
	void clear();

	/**
	 * @brief Set changed event callback.
	 */
	void setOnChanged(std::function<void(PhDialerEntry&)> callback);

private:
	virtual Evas_Object *onCreate(Evas_Object *parent, void *param);

	static void onChanged(void *data, Evas_Object *obj, void *event_info);

	std::function<void(PhDialerEntry&)> m_Changed;
};

#endif /* _PH_DIALER_ENTRY_H_ */
