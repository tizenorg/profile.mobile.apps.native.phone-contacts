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

#ifndef _WPOPUP_H_
#define _WPOPUP_H_

#include "WControl.h"
#include <functional>
#include <Elementary.h>

/**
* @class	WPopup
* @brief	This class is the pop-up class to make easy to create pop-up.
*
*
* The %WPopup is the general pop-up class.
*/
class WAPP_ASSIST_EXPORT WPopup : public WControl
{
public:
	/**
	 * This is the default constructor for this class.
	 *
	 */
	WPopup();
	/**
	 * This is the constructor with title and content string.
	 *
	 */
	WPopup( const char* title, const char* textContent );
public:
	/**
	 * Sets the title string of the pop-up.
	 *
	 * @param[in]	title	title text string
	 *
	 * @see setContent()
	 */
	void setTitle( const char* title );
	/**
	 * Sets the content string of the pop-up.
	 *
	 * @param[in]	text	content text string
	 *
	 * @see other setContent()
	 */
	void setContent( const char* text );
	/**
	 * Sets the content of the pop-up with WControl instance.
	 * This instance life-time will be attached to WPopup instance.
	 *
	 * @param[in]	control	WControl instance to be set
	 *
	 * @see other setContent()
	 */
	void setContent( WControl* control );
	/**
	 * Sets the creation function of the content Evas Object of the pop-up.
	 *
	 * @param[in]	contentCreateCb	content creation function object. At this function, it should return evas object created for content.
	 *
	 * @see other setContent()
	 */
	void setContent( const std::function<Evas_Object* (Evas_Object* parent)> contentCreateCb );

	void setOrient(Elm_Popup_Orient orient);
	/**
	 * Sets the string ID's domain name to be automatically translated on language change. The text content and title text should be set with string ID.
	 *
	 * @param[in]	textDomain	text domain name
	 *
	 * @see setTitle( const char* title), setContent( const char* text)
	 */
	void setTextTranslatable( const char* textDomain );
	/**
	 * Add buttons with button text string and button pressed callback function.
	 *
	 * @param[in]	buttonText	text string of button
	 * @param[in]	buttonCb	callback function object to be called when button is pressed.
	 *                          At this function, if it returns true, pop-up will be deleted. If not, pop-up will not.
	 *
	 * @return true if succeeded
	 */
	bool addButton(const char* buttonText, const std::function<void (bool* destroyPopup)> buttonCb, const std::function<void (Evas_Object* button)> setPropertyCb = NULL );
	/**
	 * Add handler function when back button is pressed.
	 * If it is not set, pop-up will be closed.
	 *
	 * @param[in]	backCb	call back function object.
	 *                      If it returns true, pop-up will be closed, if not, remained.
	 *
	 */
	void setOnBack( const std::function<void (bool* destroyPopup)> backCb );
	/**
	 * Add handler function to set more properties of pop-up.
	 * It will be called just after pop-up evas object is created in onCreate method.
	 *
	 * @param[in]	morePropertiesCb	callback function object
	 *
	 * @return true if succeeded
	 */
	void setOnMoreProperties( const std::function<void(Evas_Object* popup)>& morePropertiesCb );

protected:
	/**
	 * Override it, if you want to create your own pop-up. WPopup::onCreate will be called at the end of that function.
	 * This method comes from WControl class.
	 *
	 * @param[in]	parent	parent evas object
	 * @param[in]	param	user data
	 *
	 * @return Evas object created
	 */
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);
	/**
	 * It is called when pop-up instance is deleted. This method comes from WControl class.
	 *
	 * @see onCreate()
	 */
	virtual void onDestroy();

protected:
	/**
	 * This is the destructor for this class.
	 *
	 */
	virtual ~WPopup();
private:
	WDECLARE_PRIVATE_IMPL(WPopup);
	WDISABLE_COPY_AND_ASSIGN(WPopup);
};

#endif // _WPOPUP_H_
