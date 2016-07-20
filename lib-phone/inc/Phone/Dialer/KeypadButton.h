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

#ifndef PHONE_DIALER_KEYPAD_BUTTON_H
#define PHONE_DIALER_KEYPAD_BUTTON_H

#include "Ui/Button.h"

namespace Phone
{
	namespace Dialer
	{
		/**
		 * @brief Dialer keypad button
		 */
		class KeypadButton : public Ui::Button
		{
		public:
			/**
			 * @brief Phone dialer key ID.
			 */
			enum Id
			{
				ID_1,       /**< Dialer key 1 */
				ID_2,       /**< Dialer key 2 */
				ID_3,       /**< Dialer key 3 */
				ID_4,       /**< Dialer key 4 */
				ID_5,       /**< Dialer key 5 */
				ID_6,       /**< Dialer key 6 */
				ID_7,       /**< Dialer key 7 */
				ID_8,       /**< Dialer key 8 */
				ID_9,       /**< Dialer key 9 */
				ID_STAR,    /**< Dialer key * */
				ID_0,       /**< Dialer key 0 */
				ID_SHARP,   /**< Dialer key # */
			};

			/**
			 * @brief Create keypad button.
			 * @param   id  Key ID
			 */
			KeypadButton(Id id);

			/**
			 * @return Button character value.
			 */
			char getValue() const;

			/**
			 * @return Button ID.
			 */
			Id   getId() const;

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;

			Id m_Id;
			const struct KeypadButtonData *m_Data;
		};
	}
}

#endif /* PHONE_DIALER_KEYPAD_BUTTON_H */
