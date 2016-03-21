/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef APP_WIDGET_H
#define APP_WIDGET_H

#include <widget_app.h>
#include <widget_service.h>

namespace Ui
{
	class Window;
}

namespace App
{
	/**
	 * @brief Homescreen widget base class.
	 */
	class EXPORT_API Widget
	{
	public:
		Widget();
		virtual ~Widget();

		/**
		 * @brief Create widget.
		 * @param[in]   context Widget context
		 * @param[in]   content Widget content bundle
		 */
		void create(widget_context_h context, bundle *content);

		/**
		 * @brief Resize widget.
		 * @param[in]   width   Widget new width
		 * @param[in]   height  Widget new height
		 */
		void resize(int width, int height);

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
		Ui::Window *getWindow() const;

	protected:
		/**
		 * @brief Save widget content bundle.
		 * @param[in]   content Content to save
		 */
		void saveContent(bundle *content);

		/**
		 * @brief Called when widget is created.
		 * @param[in]   content Widget content bundle
		 */
		virtual void onCreate(bundle *content) { }

		/**
		 * @brief Called when widget is destroyed.
		 * @param[in]   reason  Reason for destruction
		 */
		virtual void onDestroy(widget_app_destroy_type_e reason) { }

		/**
		 * @brief Called when widget is paused.
		 */
		virtual void onPause() { }

		/**
		 * @brief Called when widget is resumed.
		 */
		virtual void onResume() { }

		/**
		 * @brief Called when widget is resized.
		 * @param[in]   width   Widget new width
		 * @param[in]   height  Widget new height
		 */
		virtual void onResize(int width, int height) { }

		/**
		 * @brief Called when widget is updated.
		 * @param[in]   force   Whether widget was forcefully updated
		 */
		virtual void onUpdate(int force) { }

	private:
		friend class WidgetApplication;

		widget_context_h m_Context;
		int m_Width;
		int m_Height;
		Ui::Window *m_Window;
	};
}

#endif /* APP_WIDGET_H */
