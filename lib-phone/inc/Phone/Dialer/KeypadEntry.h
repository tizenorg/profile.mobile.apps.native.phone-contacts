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

#ifndef PHONE_DIALER_KEYPAD_ENTRY_H
#define PHONE_DIALER_KEYPAD_ENTRY_H

#include "Ui/Control.h"
#include <string>
#include <functional>

namespace Phone
{
	namespace Dialer
	{
		/**
		 * @brief Dialer keypad entry.
		 */
		class KeypadEntry : public Ui::Control
		{
		public:
			/**
			 * @brief Entry text changed callback
			 */
			typedef std::function<void()> ChangedCallback;

			/**
			 * @return Entered number
			 */
			std::string getNumber();

			/**
			 * @brief Set number
			 */
			void setNumber(const std::string &number);

			/**
			 * @brief Insert character at the cursor position.
			 * @param   c   Character to insert
			 */
			void insert(char c);

			/**
			 * @brief Remove last character from the current entry.
			 */
			void popBack();

			/**
			 * @brief Erase current entry.
			 */
			void clear();

			/**
			 * @brief Set changed event callback.
			 */
			void setChangedCallback(ChangedCallback callback);

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			void onChanged(Evas_Object *obj, void *event_info);

			ChangedCallback m_OnChanged;
		};
	}
}

#endif /* PHONE_DIALER_KEYPAD_ENTRY_H */
