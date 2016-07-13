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

#ifndef UX_EDITFIELD_POPUP_H
#define UX_EDITFIELD_POPUP_H

#include "Ui/Popup.h"

namespace Ui
{
	class Editfield;
}

namespace Ux
{
	/**
	 * @brief Popup with editfield.
	 */
	class EditfieldPopup : public Ui::Popup
	{
	public:
		/**
		 * @brief Called when user confirmation button is pressed.
		 * @param[in]   Inputed text
		 */
		typedef std::function<void(const char *)> ResultCallback;

		/**
		 * @brief Translatable strings table for popup elements.
		 */
		struct Strings
		{
			const char *popupTitle;   /**< Popup title */
			const char *guideText;    /**< Entry guide text */
			const char *buttonDone;   /**< "Done" button text */
			const char *buttonCancel; /**< "Cancel" button text */
		};

		EditfieldPopup();

		/**
		 * @brief Set translatable strings for popup.
		 * @remark Should be called before create().
		 * @param[in]   strings    Translatable strings table
		 */
		void setStrings(Strings strings);

		/**
		 * @brief Set result callback.
		 * @param[in]   callback    Result callback
		 */
		void setResultCallback(ResultCallback callback);

		/**
		 * @return Contained editfield.
		 */
		Ui::Editfield *getEditfield() const;

	protected:
		/**
		 * @brief Creates editfield and popup buttons.
		 * @see Control::onCreated()
		 */
		virtual void onCreated() override;

	private:
		bool onDoneButtonPressed();
		void onDoneKeyPressed(Evas_Object *entry, void *eventInfo);
		static void onEntryChanged(Evas_Object *button, Evas_Object *entry, void *eventInfo);

		Ui::Editfield *m_Editfield;
		ResultCallback m_OnResult;
		Strings m_Strings;
	};
}

#endif /* UX_EDITFIELD_POPUP_H */
