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

#ifndef UI_EDITFIELD_H
#define UI_EDITFIELD_H

#include "Ui/Control.h"

namespace Ui
{
	/**
	 * @brief Editfield layout with entry and "clear" button
	 */
	class EXPORT_API Editfield : public Control
	{
	public:
		/**
		 * @brief Allows method overload instead of shadowing
		 */
		using Control::create;

		/**
		 * @brief Create editfield with guide text.
		 * @param[in]   parent      Editfield parent
		 * @param[in]   guideText   Text to be displayed when entry is empty
		 * @return Created editfield.
		 */
		static Editfield *create(Evas_Object *parent, const char *guideText);

		/**
		 * @brief Set entry guide text.
		 * @param[in]   guideText   Text to be displayed when entry is empty
		 */
		void setGuideText(const char *guideText);

		/**
		 * @return Contained entry.
		 */
		Evas_Object *getEntry();

		/**
		 * @return Clear button.
		 */
		Evas_Object *getClearButton();

	private:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;

		static void onChanged(Evas_Object *layout, Evas_Object *entry);
		static void onFocused(Evas_Object *layout);
		static void onUnfocused(Evas_Object *layout);
		static void onClearPressed(Evas_Object *entry);
	};
}

#endif /* UI_EDITFIELD_H */
