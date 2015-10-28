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

#ifndef _WWINDOW_H_
#define _WWINDOW_H_

#include <Elementary.h>
#include "WDefine.h"

class IWUiObject;

/**
* @class	WWindow
* @brief	This class represents window and base layout evas object.
*
* The %WWindow manages window evas object and common base evas objects.
*/
class WAPP_ASSIST_EXPORT WWindow
{
public:
	/**
	 * Creates the window
	 *
	 * @return true if the windows created successfully, else false
	 */
	bool create();

	/**
	 * Destroys the window class instance and delete window evas object.
	 * "onDestroy" will be called subsequently.
	 *
	 * @see onDestroy()
	 */
	void destroy();

	/**
	 * Gets the class name of this class
	 *
	 * @return The class name
	 */
	virtual const char* getClassName() const;

	/**
	 * Gets the evas object of the window
	 *
	 * @return The pointer to the evas object of the window
	 * @see onCreateWin()
	 */
	Evas_Object* getEvasObj() const;

	/**
	 * Gets the base layout evas object
	 *
	 * @return The pointer to the evas object of the layout
	 * @see onCreateBaseLayout()
	 */
	Evas_Object* getBaseLayoutEvasObj() const;

	/**
	 * Gets the conformant evas object.
	 *
	 * @return The pointer to the evas object of the conformant
	 */
	Evas_Object* getConformantEvasObj() const;

	/**
	 * Attaches base UI object, which can be WNaviframe or WView instance.
	 *
	 * @param[in]	baseObj	IWUiObject instance to be attached
	 * @param[in]	creationParam	parameter to be passed as parameter of create method of the IWUiObject class
	 *
	 * @return true if the base ui object attached sucessfully, else false
	 * @see getBaseUiObject()
	 */
	bool attachBaseUiObject( IWUiObject* baseObj, void* creationParam = NULL );

	/**
	 * Gets the base UI object attached.
	 *
	 * @return The pointer to the base ui object
	 * @see attachBaseUiObject()
	 */
	IWUiObject* getBaseUiObject() const;

	/**
	 * Detaches the base UI object.
	 *
	 * @return the base UI object, which should be deleted explicitly later.
	 */
	IWUiObject* detachBaseUiObject();

	/**
	 * Destroys the base UI object.
	 *
	 */
	void destroyBaseUiObject();

protected:
	/**
	 * Called when create() method is called to create window.
	 * Implementation should assign window, conformant and base layout.
	 *
	 * @param[out]  win     Window object
	 * @param[out]  conf    Conformant object
	 * @param[out]  layout  Base layout object
	 * @return Object which determines current object's lifetime
	 * @see create()
	 */
	virtual Evas_Object* doCreate(Evas_Object*& win, Evas_Object*& conf, Evas_Object*& layout) = 0;

	/**
	 * Called when the window is destroyed
	 * Override this method to do something on destruction.
	 *
	 */
	virtual void onDestroy();


protected:
	/**
	 * This is the destructor for this class.
	 *
	 */
	virtual ~WWindow();

	WDECLARE_PRIVATE_IMPL(WWindow);
	WDISABLE_COPY_AND_ASSIGN(WWindow);
	WWindow(__WWindowImpl*);
};
#endif /* _WWINDOW_H_ */
