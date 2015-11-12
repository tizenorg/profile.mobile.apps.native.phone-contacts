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

#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "Ui/Control.h"
#include <functional>

namespace Ui
{
	/**
	 * @brief Provides convenient press and longpress events handling.
	 */
	class EXPORT_API Button : public Control
	{
	public:
		Button();
		virtual ~Button() override;

		/**
		 * @brief Set press-event callback.
		 */
		void setPressedCallback(std::function<void(Button &)> callback);

		/**
		 * @brief Set longpress-event callback.
		 * @details Callback should return true if the event was handled.
		 *          If callback returns false onPressed event will be delivered
		 *          as well when the button is released.
		 */
		void setLongpressedCallback(std::function<bool(Button &)> callback);

	private:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;
		virtual void onCreated() override;

		void onMouseDown(Evas *evas, Evas_Object *obj, void *eventInfo);
		void onMouseUp(Evas *evas, Evas_Object *obj, void *eventInfo);
		void onMouseOut(Evas *evas, Evas_Object *obj, void *eventInfo);

		void resetTimer();
		Eina_Bool onTimeout();

		Ecore_Timer *m_Timer;
		bool m_IsLongpressed;

		std::function<void(Button &)> m_OnPressed;
		std::function<bool(Button &)> m_OnLongpressed;
	};
}

#endif /* UI_BUTTON_H */
