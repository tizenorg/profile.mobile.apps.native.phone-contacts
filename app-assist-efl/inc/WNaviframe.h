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

#ifndef _WNAVI_FRAME_H_
#define _WNAVI_FRAME_H_

#include <functional>
#include <Evas.h>
#include <Eina.h>
#include <Elementary.h>
#include "IWUiObject.h"
#include "WWindow.h"

// forward declaration
class WView;

/**
* @class	WNaviframe
* @brief	This class is the wrapper class of naviframe elementary.
*
*
* The %WNaviframe class is to use naviframe elementary.
*/
class WAPP_ASSIST_EXPORT WNaviframe : public IWUiObject
{
public:
	/**
	 * This is the default constructor for this class.
	 *
	 */
	WNaviframe();
	/**
	 * Gets the class name.
	 *
	 * @return The class name
	 */
	virtual const char* getClassName();

public:
	/**
	 * Initializes and creates the evas object.
	 * "onCreate" will be called subsequently.
	 *
	 * @param[in]	parent	the parent evas object
	 * @param[in]	param	user data
	 *
	 * @return	true if the creation is succeeded, else false
	 * @see onCreate()
	 */
	bool create( Evas_Object* parent, void* param );

	/**
	 * Destroy the instance. "onDestroy" will be called subsequently.
	 * The naviframe elementary will be deleted together.
	 *
	 * @see onDestroy()
	 */
	void destroy();

	/**
	 * Gets the evas object of this navi frame
	 *
	 * @return The pointer to the evas object
	 */
	Evas_Object* getEvasObj();

	/**
	 * Sets the name of this instance.
	 *
	 * @param[in]	name	The name to be set
	 */
	void setName( const char* name);

	/**
	 * Gets the name of this instance.
	 *
	 * @return The name of the navi frame
	 */
	const char* getName();

	/**
	 * Pushes the view instance into naviframe. "onPushToNaviframe" will be called subsequently.
	 * If the view is yet called "create", it will be called internally.
	 *
	 * @param[in]	view	the pointer to the view instance to be pushed
	 * @param[in]	viewName	the view instance name to be set when it is pushed.
	 * @param[in]	viewParam	the user data to be passed as parameter of "create" method of the view.
	 *
	 * @return	true if succeed, else false
	 * @see onPushToNaviframe
	 */
	bool push( WView* view, const char* viewName = NULL, void* viewCreationParam = NULL );

	/**
	 * Destroy all views stacked on naviframe.
	 *
	 */
	void destroyAllViews();

public:
	/**
	 * Sets the handler function pointer to override virtual function onCreate without defining a new inherited class.
	 * If it is set, onCreate method will not be called.
	 *
	 * @param[in]	handlerFunc	The pointer to handler function
	 * @see onCreate()
	 */
	void setOnCreate( const std::function<Evas_Object*(Evas_Object*, void*)>& handlerFunc);

	/**
	 * Sets the handler function pointer to override virtual function onLastItemPop without defining a new inherited class.
	 * If it is set, onLastItemPop method will not be called.
	 *
	 * @param[in]	handlerFunc	The pointer to handler function
	 * @see onLastItemPop()
	 */
	void setOnLastItemPop( const std::function<void(bool*)>& handlerFunc);
protected:

	/**
	 * Override this method to create your own naviframe elementary.
	 *
	 * @param[in]	parent	The pointer to the parent evas object
	 * @param[in]	param	user data
	 * @return The pointer to naviframe evas object created
	 */
	virtual Evas_Object* onCreate( Evas_Object* parent, void* param);
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
	virtual void onDestroy();
	/**
	 * Override this method to do something before pushing the view to the naviframe.
	 * In the overridden method, WNaviframe::onPush should be called to do the base operation.
	 *
	 * @param[in]	view	The pointer to the view instance to be pushed
	 * @param[in]	viewParam	user data to be passed from "push". It is passed to the "create" method of the WView class
	 *
	 * @return result
	 * @see push
	 */
	virtual bool onPush( WView* view, void* viewParam);

	/**
	 * Override this method to do something before popping the view from the naviframe.
	 * If you override this method, call WNaviframe::onItemPop at the bottom of your method to do the base operations.
	 *
	 * @param[in]	view	The pointer to the view instance to be popped
	 * @param[in]	item	The pointer to the navi item
	 *
	 * @return	true the view will be popped out, false the view will not be popped.
	 * @see onLastItemPop()
	 */
	virtual bool onItemPop( WView* view, Elm_Object_Item* item);

	/**
	 * Override this method to change how the application should work when last view is popped from naviframe.
	 * If you don't override it, the application will go background by default.
	 * Even though onLastItemPop is called, "onItemPop" will be called before.
	 *
	 * @param[out]	popOut	Set true, if you will pop out the last view and end the application. If not, set false before return
	 * @see onItemPop()
	 */
	virtual void onLastItemPop( bool* popOut );

	/**
	 * Gets the pointer to the WWindow class instance.
	 *
	 * @return The pointer to the window instance.
	 * @see getUgWindow()
	 */
	WWindow* getWindow();

protected:
	/**
	 * This is the destructor for this class.
	 *
	 */
	virtual ~WNaviframe();
private:
	static Eina_Bool __popCb( void* data, Elm_Object_Item* item);
private:
	WDECLARE_PRIVATE_IMPL(WNaviframe);
	WDISABLE_COPY_AND_ASSIGN(WNaviframe);

};

#endif /* _WNAVI_FRAME_H_ */
