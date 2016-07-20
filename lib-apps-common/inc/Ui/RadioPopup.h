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

#ifndef UI_RADIO_POPUP_H
#define UI_RADIO_POPUP_H

#include "Ui/ListPopup.h"
#include <string>

namespace Ui
{
	/**
	 * @brief Radio popup
	 */
	class EXPORT_API RadioPopup : public ListPopup
	{
	public:
		RadioPopup();

		/**
		 * @brief Set selected item
		 * @param[in]   value     Selected item value
		 */
		void setSelectedItem(int value);

	private:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;
		virtual Evas_Object *getItemContent(void *data, const char *part) override;
		virtual void onItemSelected(void *data) override;

		Evas_Object *m_RadioGroup;
	};
}
#endif /* UI_RADIO_POPUP_H */
