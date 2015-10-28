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

#ifndef _WPAGE_SLIDE_VIEW_H_
#define _WPAGE_SLIDE_VIEW_H_

#include "WView.h"
#include "WControl.h"
#include <Elementary.h>
#include <functional>

/**
* @class	WPageSlideView
* @brief	This class provides slide view with pages
*
*
* The %WPageSlideView is the class to provide slide page view.
*/
class WAPP_ASSIST_EXPORT WPageSlideView : public WView
{
public:
	/**
	 * This is the default constructor for this class.
	 *
	 */
	WPageSlideView();

	/**
	 * Retuns the class name string
	 *
	 * @return class name
	 */
	virtual const char* getClassName();

public:
	/**
	 * Attaches WControl ui object as a page.
	 *
	 * @param[in]	control	WControl instance to be attached.
	 *
	 * @return true if succeeded.
	 */
	bool appendPage( WControl* content);

	/**
	 * Attaches WView ui object as a page
	 *
	 * @param[in]	view	WView object to be attached.
	 *
	 * @return true if succeeded.
	 * @see WView::onSetAsPageContent()
	 */
	bool appendPage( WView* content);

	/**
	 * Attaches empty ui object as a page
	 *
	 * @return true if succeeded.
	 */
	bool appendPage();

	/**
	 * Sets the page WView. If the previous page WView is exist, it returns false.
	 *
	 * @param[in]	index	the index of the page
	 * @param[in]	view	WView object
	 */
	bool setPageContent(int index, WView* view);

	/**
	 * Sets the page WControl. If the previous page WControl is exist, it returns false.
	 *
	 * @param[in]	index	the index of the page
	 * @param[in]	control	WControl object
	 */
	bool setPageContent(int index, WControl* control);

	/**
	 * Returns the number of page.
	 *
	 * @return the number of page
	 */
	int getPageCount();

	/**
	 * Gets the page instance.
	 *
	 * @param[in]	index	index of the page
	 *
	 * @return UI object
	 */
	IWUiObject* getPageContent( int index );

	/**
	 * Gets the current page index.
	 *
	 * @return The current page index
	 */
	int getCurrentPageIndex();

	/**
	 * Gets the scroller evas object.
	 *
	 *
	 * @return The pointer to the evas object of the scroller
	 */
	Evas_Object* getScroller();

	/**
	 * Sets the flag whether the scroller is shown or not.
	 *
	 * @param[in]	show	true if it should be shown.
	 */
	void setShowScroll(bool show);

	/**
	 * Sets the current page
	 *
	 * @param[in]	index	the index of the page
	 */
	void setCurrentPage(int index);

	/**
	 * Changes to the page
	 *
	 * @param[in]	index			the index of the page
	 * @param[in]	withAnimation	flag if animation is shown or not.
	 */
	void showPage(int index, bool withAnimation = false);


public:
	/**
	 * Sets callback function object to be called when scroller is created.
	 * Use this to set additional property to the scroller.
	 *
	 * @param[in]	handlerFunc	callback function object
	 *
	 * @see onInitScroller()
	 */
	void setOnInitScroller( const std::function<void (Evas_Object*)>& handlerFunc );

	/**
	 * Sets callback function object to be called when scroll animation completed.
	 *
	 * @param[in]	scrollAnimStopCb	callback function object
	 */
	void setOnScrollAnimStop( const std::function<void()>& scrollAnimStopCb );
protected:
	/**
	 * Unsets the page object. The page object is not deleted.
	 * It should be used with setPageObject method to reset the pages.
	 *
	 * @param[in]	index	the index of the page
	 *
	 * @see setPageObject
	 */
	void unsetPageObject(int index);

	/**
	 * Sets the page object. The previous page object to be set will be deleted internally.
	 *
	 * @param[in]	index	the index of the page
	 * @param[in]	control	control object
	 *
	 * @see unsetPageObject
	 */
	void setPageObject(int index, WControl* control);

protected:
	/**
	 * Called when the view is created. It comes from WView class.
	 *
	 * @param[in]	parent	parent evas object
	 * @param[in]	viewParam	user parameter
	 */
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam );

	/**
	 * Called when the layout is created. It comes from WView class.
	 *
	 * @param[in]	parent	parent evas object
	 *
	 * @return The evas object of layout to be created.
	 */
	virtual Evas_Object* onCreateLayout( Evas_Object* parent);

	/**
	 * Called when the scroller is created.
	 * Override this to set additional property to the scroller.
	 *
	 * @param[in]	scroller	scroller evas object
	 *
	 * @see setOnInitScroller()
	 */
	virtual void onInitScroller( Evas_Object* scroller );
protected:
	/**
	 * This is the destructor for this class.
	 *
	 */
	virtual ~WPageSlideView();
private:
	WDECLARE_PRIVATE_IMPL(WPageSlideView);
	WDISABLE_COPY_AND_ASSIGN(WPageSlideView);
};

#endif /* _WPAGE_SLIDE_VIEW_H_ */
