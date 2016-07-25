/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef UI_POPUP_H
#define UI_POPUP_H

#include "Ui/Control.h"
#include <functional>

#define POPUP_BUTTON_MAX_COUNT 3

namespace Ui
{
	class EXPORT_API Popup : public Control
	{
	public:
		/**
		 * @brief Popup button pressed callback
		 * @return true to destroy the Popup, otherwise false
		 */
		typedef std::function<bool()> ButtonCallback;

		/**
		 * @brief Back button pressed callback
		 * @return true to destroy the Popup, otherwise false
		 */
		typedef std::function<bool()> BackCallback;

		/**
		 * @brief Create popup
		 * @param[in]   isVisible   Popup visibility on start
		 */
		Popup(bool isVisible = true);

		/**
		 * @brief Allows method overload instead of shadowing
		 */
		using Control::create;

		/**
		 * @brief Create popup with title, text and close button
		 * @param[in]   parent      Popup parent
		 * @param[in]   title       Popup title
		 * @param[in]   text        Popup text
		 * @param[in]   button_text Text for close button
		 * @return Created popup
		 */
		static Popup *create(Evas_Object *parent, const char *title,
				const char *text, const char *button_text);

		/**
		 * @brief Set Popup title
		 * @param[in]   title       Popup title
		 */
		void setTitle(const char *title);

		/**
		 * @brief Set Popup text
		 * @param[in]   text        Popup text
		 */
		void setText(const char *text);

		/**
		 * @brief Set Popup content
		 * @param[in]   content     Popup content
		 */
		void setContent(Evas_Object *content);

		/**
		 * @brief Add Popup button
		 * @param[in]   text        Button text
		 * @param[in]   callback    Button click callback
		 * @return Added button on success, otherwise nullptr
		 */
		Evas_Object *addButton(const char *text, ButtonCallback callback = nullptr);

		/**
		 * @brief Set callback on back button
		 * @param[in]   callback    Back callback
		 */
		void setBackCallback(BackCallback callback);

		/**
		 * @brief Close popup with animation.
		 */
		void close();

	protected:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;

	private:
		void onButtonPressed(Evas_Object *obj, void *eventInfo);
		void onBackPressed(Evas_Object *obj, void *eventInfo);

		ButtonCallback m_ButtonCbs[POPUP_BUTTON_MAX_COUNT];
		size_t m_ButtonCount;
		bool m_IsVisible;
		BackCallback m_OnBack;
	};
}

#endif /* UI_POPUP_H */
