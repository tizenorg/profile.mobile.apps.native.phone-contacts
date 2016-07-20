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

#ifndef UX_DATE_POPUP_H
#define UX_DATE_POPUP_H

#include "Ui/Popup.h"
#include <string>

namespace Ux
{
	/**
	 * @brief Popup with date picker.
	 */
	class EXPORT_API DatePopup : public Ui::Popup
	{
	public:
		/**
		 * @brief Date input result callback.
		 * @param[in]   Inputed date
		 */
		typedef std::function<void(const tm &)> ResultCallback;

		/**
		 * @brief Translatable strings table for popup elements.
		 */
		struct Strings
		{
			const char *popupTitle;   /**< Popup title */
			const char *buttonDone;   /**< "Done" button text */
			const char *buttonCancel; /**< "Cancel" button text */
		};

		/**
		 * @brief Create date popup.
		 * @param[in]   format  Data format for elm_datetime_format_set()
		 * @param[in]   date    Initial date value
		 */
		DatePopup(const char *format, tm date);

		/**
		 * @brief Set translatable strings for popup.
		 * @remark Should be called before create().
		 * @param[in]   strings    Translatable strings table
		 */
		void setStrings(Strings strings);

		/**
		 * @brief Set date input result callback.
		 * @param[in]   callback    Callback to be called when user confirmed inputed date
		 */
		void setResultCallback(ResultCallback callback);

		/**
		 * @return Contained date picker.
		 */
		Evas_Object *getDatePicker() const;

	protected:
		/**
		 * @brief Creates date picker and popup buttons.
		 * @see Control::onCreate()
		 */
		virtual void onCreated() override;

	private:
		bool onDonePressed();

		std::string m_Format;
		tm m_Date;

		Evas_Object *m_DatePicker;
		ResultCallback m_OnResult;
		Strings m_Strings;
	};
}

#endif /* UX_DATE_POPUP_H */
