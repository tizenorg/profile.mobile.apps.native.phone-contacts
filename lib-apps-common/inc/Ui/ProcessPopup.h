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

#ifndef UI_PROCESS_POPUP_H
#define UI_PROCESS_POPUP_H

#include "Ui/Popup.h"

namespace Ui
{
	class EXPORT_API ProcessPopup : public Popup
	{
	public:
		enum Size
		{
			SizeSmall,
			SizeMedium
		};

		/**
		 * @brief Create popup.
		 * @param[in]   size    Popup size
		 */
		explicit ProcessPopup(Size size = SizeMedium);

		/**
		 * @brief Set Popup text.
		 * @param[in]   text    Popup text
		 */
		void setText(const char *text);

	protected:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;

	private:
		Size m_Size;
		Evas_Object *m_Layout;
	};
}

#endif /* UI_PROCESS_POPUP_H */
