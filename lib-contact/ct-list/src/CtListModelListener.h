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

#ifndef _CT_LIST_MODEL_LISTENER_H_
#define _CT_LIST_MODEL_LISTENER_H_

/**
 * @brief Contacts list model listener.
 */
class CtListModelListener
{
public:
	virtual ~CtListModelListener(){};

	/**
	 * @brief Called when filter for contacts data is changed.
	 */
	virtual void onFilterChanged() = 0;
};

#endif // _CT_LIST_MODEL_LISTENER_H_

