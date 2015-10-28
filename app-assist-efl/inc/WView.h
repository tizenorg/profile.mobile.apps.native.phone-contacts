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

#ifndef _WVIEW_H_
#define _WVIEW_H_

#include <functional>
#include <memory>
#include <Evas.h>
#include <Elementary.h>
#include "WNaviframe.h"
#include "WWindow.h"
#include "WControl.h"

/**
* @class	WView
* @brief	This class means the view represented by an evas object, which can be pushed into naviframe elementary.
*
* The %WView class keeps an evas object and provides methods to handle the case when the view is pushed into naviframe.
*/
class WAPP_ASSIST_EXPORT WView : public WControl
{
public:
	/**
	 * This is the default constructor for this class.
	 *
	 */
	WView();

	/**
	 * Constructor
	 *
	 * @param[in]	name	the instance name to be set
	 */
	explicit WView( const char* name );

	/**
	 * Gets the class name.
	 *
	 */
	virtual const char* getClassName();

	/**
	 * @see WControl::create()
	 */
	virtual bool create(Evas_Object* parent, void* param);

	/**
	 * Destroys the control. The linked evas object will be delete together.
	 * "onDestroy" will be called subsequently.
	 *
	 * @see onDestroy()
	 */
	virtual void destroy();

	/**
	 * Sets the title string of the view. Only on initial creation time, it is applied to the evas object.
	 *
	 * @param[in]	title	string of the title to be set
	 *
	 */
	void setTitle( const char* title );

	/**
	 * Gets the title string
	 *
	 * @return title string
	 */
	const char* getTitle();

	/**
	 * Gets the WWindow class instance.
	 *
	 * @return The pointer to the window.
	 * @see getUgWindow()
	 */
	WWindow* getWindow();

	/**
	 * Gets the WNaviframe class instance.
	 *
	 * @return the pointer to the WNaviframe class instance. If this view instance is not pushed into naviframe, it will be NULL.
	 */
	WNaviframe* getNaviframe();

	/**
	 * Sets the handler function pointer to override virtual function onPushed without defining a new inherited class.
	 * If it is set, onPushed method will not be called.
	 *
	 * @param[in]	handlerFunc	The pointer to handler function
	 * @see onPushed()
	 */
	void setOnPushed( const std::function<void (Elm_Object_Item*)>& handlerFunc );

protected:
	/**
	 * Called when create method is called.
	 * Override this method to create your own view.
	 *
	 * @param[in]	parent	parent evas object
	 * @param[in]	viewParam	user data
	 *
	 * @return the most parent evas object created in this method. It will be related to the class instance.
	 */
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam );

	/**
	 * Called when this view instance evas object should be pushed into naviframe.
	 * Override this method to do something and push the view into naviframe by itself.
	 *
	 * @param[in]	naviFrame	naviframe evas object
	 *
	 * @return navi item resulted from pushing the view to the naviframe.
	 * @see %WNaviframe::push()
	 * @see onPushed()
	 */
	virtual Elm_Object_Item* onPushToNaviframe( Evas_Object* naviFrame);

	/**
	 * Called when this view instance evas object is pushed into naviframe.
	 * Override this method to do something with navi item.
	 *
	 * @param[in]	naviItem	navi item resulted from naviframe push
	 * @see %WNaviframe::push()
	 * @see onPushToNaviframe()
	 */
	virtual void onPushed(Elm_Object_Item* naviItem);

	/**
	 * Called when the view evas object is popped from naviframe.
	 * Override this method to do something before it is popped out.
	 *
	 * @return false, if you want to keep the view instead of being popped out.
	 */
	virtual bool onPop();

	/**
	 * Called when this view instance evas object is attached to WPageSlideView.
	 * Override this method to do something with navi item after being attached.
	 *
	 * @param[in]	parentNaviItem	navi item of WPageSlideView object pushed into naviframe
	 * @see onPushed()
	 */
	virtual void onSetAsPageContent(Elm_Object_Item* parentNaviItem);

	/**
	 * Gets a navi item.
	 *
	 * @return The pointer to navi item. If this view instance is not pushed into naviframe, it will be NULL.
	 */
	Elm_Object_Item* getNaviItem();

	/**
	 * Pops out this view instance evas object from naviframe.
	 *
	 * @return true, if succeeded
	 */
	bool popOut();

	/**
	 * Create more button on navi item.
	 *
	 * @param[in]	naviItem
	 * @param[in]	clickedCb
	 * @param[in]	userData
	 * @see %WNaviframe::push()
	 * @see onPushed()
	 */
	bool enableMoreButton( Elm_Object_Item* naviItem, Evas_Smart_Cb clickedCb, void* userData );

	/**
	 * This is the destructor for this class.
	 *
	 */
	virtual ~WView();

private:
	bool __callPushedHandlerFunc(Elm_Object_Item* naviItem);
	void __destroy();

	WDECLARE_PRIVATE_IMPL(WView);
	WDISABLE_COPY_AND_ASSIGN(WView);
	friend class WNaviframe;
	friend class WPageSlideView;
	friend void WView_setContainerProperties( WView* view, WWindow* win, WNaviframe* naviframe );
	friend void WView_setContainerNaviItemProperty( WView* view, Elm_Object_Item* item );
};


/**
 * Get the view class instance related to evas object.
 *
 * @param[in]	obj	evas object
 *
 * @return NULL, if no view class instance is related.
 */
WAPP_ASSIST_EXPORT WView* WView_getInstanceFromEvasObj(Evas_Object* obj);
/**
 * Sets the container class properties to the view.
 * It will be used only to create your own container view class and set its properties to child views, like WPageSlideView.
 *
 * @param[in]	view	WView instance to set these properties
 * @param[in]	win		WWindow instance
 * @param[in]	naviframe	WNaviframe instance
 *
 */
WAPP_ASSIST_EXPORT void WView_setContainerProperties( WView* view, WWindow* win, WNaviframe* naviframe );
/**
 * Sets the container class navi item property to the view.
 * It will be used only to create your own container view class and set its properties to child views, like WPageSlideView.
 *
 * @param[in]	view	WView instance to set these properties
 * @param[in]	item	navi item pointer of container view which is pushed into naviframe.
 *
 */
WAPP_ASSIST_EXPORT void WView_setContainerNaviItemProperty( WView* view, Elm_Object_Item* item );


#endif /* _WVIEW_H_ */
