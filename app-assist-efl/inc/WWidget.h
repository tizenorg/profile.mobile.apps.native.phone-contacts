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

#ifndef _WWIDGET_H_
#define _WWIDGET_H_

#include <widget_app.h>
#include <widget_service.h>
#include "WDefine.h"

class WWindow;

/**
* @class	WWidget
* @brief	This class is a widget representation interface
*
* The %WWidget class designated to show the content of app to the homescreen
*/
class WAPP_ASSIST_EXPORT WWidget
{
public:
	/**
	 * Constructor
	 *
	 */
	WWidget();

	/**
	 * This is the destructor for this class.
	 *
	 */
	virtual ~WWidget();

	/**
	 * @return Widget width.
	 */
	int getWidth() const;

	/**
	 * @return Widget height.
	 */
	int getHeight() const;

	/**
	 * @return Widget type.
	 * @see widget_size_type_e
	 */
	widget_size_type_e getType() const;

	/**
	 * @return Widget window.
	 */
	WWindow *getWindow() const;

	/**
	 * @brief Save widget content bundle.
	 * @param[in]	content 	Content to save
	 */
	void saveContent(bundle *content);

	/**
	 * @brief Create widget.
	 * @param[in]	context 	Widget context
	 * @param[in]	content 	Widget content bundle
	 * @param[in]	w   		Widget width
	 * @param[in]	h   		Widget height
	 */
	void create(widget_context_h context, bundle *content, int w, int h);

	/**
	 * @brief Resize widget.
	 * @param[in]	w	Widget new width
	 * @param[in]	h	Widget new height
	 */
	void resize(int w, int h);

	/**
	 * @brief Called when widget is created.
	 * @param[in]	content 	Widget content bundle
	 * @param[in]	w   		Widget width
	 * @param[in]	h   		Widget height
	 */
	virtual void onCreate(bundle *content, int w, int h);

	/**
	 * @brief Called when widget is destroyed.
	 * @param[in]	reason	Reason for destruction
	 */
	virtual void onDestroy(widget_app_destroy_type_e reason);

	/**
	 * @brief Called when widget is paused.
	 */
	virtual void onPause();

	/**
	 * @brief Called when widget is resumed.
	 */
	virtual void onResume();

	/**
	 * @brief Called when widget is resized.
	 * @param[in]	w	Widget new width
	 * @param[in]	h	Widget new height
	 */
	virtual void onResize(int w, int h);

	/**
	 * @brief Called when widget is updated.
	 * @param[in]	force	Indicates whether widget was forcefully updated
	 */
	virtual void onUpdate(int force);

private:
	WDISABLE_COPY_AND_ASSIGN(WWidget);
	WDECLARE_PRIVATE_IMPL(WWidget);
};

#endif /* _WWIDGET_H_ */
