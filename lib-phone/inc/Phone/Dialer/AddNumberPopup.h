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

#ifndef PHONE_DIALER_ADD_NUMBER_POPUP_H
#define PHONE_DIALER_ADD_NUMBER_POPUP_H

#include "Ui/Popup.h"

namespace Phone
{
	namespace Dialer
	{
		/**
		 * @brief Add number to contacts popup.
		 */
		class AddNumberPopup : public Ui::Popup
		{
		public:
			/**
			 * @brief Create "Add to contacts" popup.
			 * @param[in]   number  Number to add
			 */
			AddNumberPopup(std::string number);

		private:
			virtual void onCreated() override;

			Evas_Object *createGenlist(Evas_Object *parent);
			Elm_Genlist_Item_Class *createItemClass();
			static char *getItemText(void *data, Evas_Object *obj, const char *part);

			static void launchContactCreate(void *data, Evas_Object *obj, void *event_info);
			static void launchContactUpdate(void *data, Evas_Object *obj, void *event_info);

			std::string m_Number;
		};
	}
}

#endif /* PHONE_DIALER_ADD_NUMBER_POPUP_H */
