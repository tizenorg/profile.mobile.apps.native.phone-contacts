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

#ifndef UI_DATE_POPUP_H
#define UI_DATE_POPUP_H

#include "Ui/Popup.h"

namespace Ui
{
	/**
	 * @brief Popup with date picker.
	 */
	class EXPORT_API DatePopup : public Popup
	{
	public:
		/**
		 * @brief Date input result callback.
		 * @param[in]   Inputed date
		 */
		typedef std::function<void(const tm&)> ResultCallback;

		/**
		 * @brief Create date popup.
		 * @param[in]   date    Initial date value
		 */
		DatePopup(tm date);

		/**
		 * @brief Set date input result callback.
		 * @param[in]   callback    Callback to be called when user confirmed inputed date
		 */
		void setResultCallback(ResultCallback callback);

	private:
		virtual void onCreated() override;
		void onSetPressed();

		tm m_Date;
		ResultCallback m_OnResult;
		Evas_Object *m_DatePicker;
	};
}

#endif /* UI_DATE_POPUP_H */
