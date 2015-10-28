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

#ifndef _WCONTROL_H_
#define _WCONTROL_H_

#include <functional>
#include <memory>
#include <Evas.h>
#include "IWUiObject.h"

/**
* @class	WControl
* @brief	This class is the UI control class linked with one evas object.
*
*
* The %WControl is the abstract class managing an evas object
*/
class WAPP_ASSIST_EXPORT WControl : public IWUiObject
{
public:
	/**
	 * This is the default constructor for this class.
	 *
	 */
	WControl();
	/**
	 * Gets class name
	 *
	 * @return The class name
	 */
	virtual const char* getClassName();
public:

	/**
	 * Initializes and creates the control. onCreate will be called subsequently
	 *
	 * @param[in]	parent	parent evas object
	 * @param[in]	param	user data
	 *
	 * @return result
	 * @see onCreate()
	 */
	virtual bool create(Evas_Object* parent, void* param );

	/**
	 * Destroys the control. The linked evas object will be delete together.
	 * "onDestroy" will be called subsequently.
	 *
	 * @see onDestroy()
	 */
	virtual void destroy();

	/**
	 * Gets evas object of this class.
	 *
	 * @return The pointer to evas object
	 */
	Evas_Object* getEvasObj();

	/**
	 * Gets evas object of this class for read-only reference.
	 *
	 * @return The read-only pointer to evas object
	 */
	const Evas_Object* getConstEvasObj() const;

	/**
	 * Sets the name of the instance.
	 *
	 * @param[in]	name	blash
	 */
	void setName( const char* name);

	/**
	 * Gets the name of the instance.
	 *
	 * @return The control name
	 */
	const char* getName();
	/**
	 * Returns weak pointer of this instance to refer it safe.
	 *
	 * @return weak pointer of this class instance
	 */
	WUiObjectPtr getWeakPtr();

	/**
	 * Attaches popup to the control.It means that it binds them to have same life-time.
	 * Internally create method of the pop-up will be called, if it is not yet created.
	 * If previous pop-up exists, it will be destroyed.
	 *
	 * @param[in]	popup pop-up instance pointer to be attached
	 *
	 * @see destroyPopup()
	 */
	void attachPopup( WControl* popup );

	/**
	 * Attaches popup to the control and hides popup. Pointer of popup will not
	 * be saved and you should delete the instance by your self.
	 * Internally create method of the pop-up will be called, if it is not yet created.
	 * If previous pop-up exists, it will be showed.
	 *
	 * @param[in]	popup pop-up instance pointer to be attached
	 *
	 */
	void attachHiddenPopup( WControl *popup );
	/**
	 * Destroys pop-up. Use this method when you want to destroy pop-up explicitly.
	 * Pop-up will be deleted when the control is deleted, although this method is not called.
	 *
	 * @see attachPopup()
	 */
	void destroyPopup();

	/**
	 * Sets the handler function pointer to override virtual function onCreate without defining a new inherited class.
	 * If it is set, onCreate method will not be called.
	 *
	 * @param[in]   handlerFunc The pointer to handler function
	 * @see onCreate()
	 */
	void setOnCreate( const std::function<Evas_Object* (Evas_Object*, void*)>& handlerFunc );

	/**
	 * Sets the handler function pointer to be called when view is being destoyed.
	 *
	 * @param[in]   handlerFunc The pointer to handler function
	 * @see onDestroy()
	 */
	void setOnDestroy( const std::function<void ()>& handlerFunc );


protected:
	/**
	 * Override this method to create evas object of this WControl class.
	 *
	 * @param[in]	parent	the parent evas object
	 * @param[in]	param	user parameter
	 *
	 * @return The pointer to evas object created, which should be the most parent evas object among them created in this method.
	 * @see create()
	 * @see onCreated()
	 */
	virtual Evas_Object* onCreate( Evas_Object* parent, void* param ) = 0;
	/**
	 * Override this method to do something after evas object is created.
	 *
	 * @see create()
	 * @see onCreate()
	 */
	virtual void onCreated(){};
	/**
	 * Override this method to do something on this instance deletion.
	 *
	 * @see destroy()
	 */
	virtual void onDestroy(){};

protected:
	/**
	 * This is the destructor for this class.
	 *
	 */
	virtual ~WControl();

private:
	WDECLARE_PRIVATE_IMPL(WControl);
	WDISABLE_COPY_AND_ASSIGN(WControl);

};
/**
 * Gets the class instance from evas object
 *
 * @param[in]	obj	evas object
 *
 * @return The pointer to WControl class instance. It will be NULL, if no instance is related.
 */
WAPP_ASSIST_EXPORT WControl* WControl_getInstanceFromEvasObj( Evas_Object* obj );

#endif /* _WCONTROL_H_ */
