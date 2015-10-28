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

#ifndef _WEDITFIELD_H_
#define _WEDITFIELD_H_

#include "WControl.h"

/**
* @class	WEditfield
* @brief	This class is a helper for creating editfields.
*
*/
class WAPP_ASSIST_EXPORT WEditfield : public WControl
{
public:
	/**
	 * @see WControl::onCreate
	 */
	Evas_Object *onCreate(Evas_Object *parent, void *param);

	/**
	 * @return Contained entry
	 */
	Evas_Object *getEntry();

	/**
	 * @return Clear button
	 */
	Evas_Object *getClearButton();
};

#endif /* _WEDITFIELD_H_ */
