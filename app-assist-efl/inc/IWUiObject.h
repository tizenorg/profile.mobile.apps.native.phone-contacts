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

#ifndef _IWUI_OBJECT_H_
#define _IWUI_OBJECT_H_

#include <Evas.h>
#include <memory>
#include "WDefine.h"

class IWUiObject;

typedef std::weak_ptr<IWUiObject> WUiObjectPtr;
/**
* @class	IWUiObject
* @brief	This class is the interface class of the view related class
*
* The %IWUiObject manages an evas object.
*/
class WAPP_ASSIST_EXPORT IWUiObject
{
public:
	/**
	 * Creates evas object linked to this instance.
	 *
	 * @param[in]	parent
	 * @param[in]	param
	 *
	 * @return true if succeed.
	 */
	virtual bool create(Evas_Object* parent, void* param )=0;

	/**
	 * Destroys this class instance and deletes evas object.
	 *
	 */
	virtual void destroy()=0;

	/**
	 * Gets the evas object created.
	 *
	 * @return evas object created
	 */
	virtual Evas_Object* getEvasObj()=0;


protected:
	/**
	 * This is the destructor for this class.
	 *
	 */
	virtual ~IWUiObject(){};

	//

};

#endif /* _IWUI_OBJECT_H_ */
