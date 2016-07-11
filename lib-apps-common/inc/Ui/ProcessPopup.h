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
		 * @param[in]   size            Popup size
		 * @param[in]   showDelayTime   Delay before showing popup
		 * @param[in]   showMinTime     Minimum time popup should be shown
		 */
		explicit ProcessPopup(Size size = SizeMedium,
				double showDelayTime = 0.2, double showMinTime = 1.0);
		virtual ~ProcessPopup() override;

		/**
		 * @brief Allows method overload instead of shadowing
		 */
		using Control::create;

		/**
		 * @brief Create process popup with text.
		 * @param[in]   parent  Popup parent
		 * @param[in]   text    Popup text
		 * @param[in]   size    Popup size
		 * @return Created popup
		 */
		static ProcessPopup *create(Evas_Object *parent, const char *text, Size size = SizeMedium);

		/**
		 * @brief Destroy the popup once minimum show time has elapsed.
		 */
		void destroy();

		/**
		 * @brief Set Popup text.
		 * @param[in]   text    Popup text
		 */
		void setText(const char *text);

	protected:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;

	private:
		Eina_Bool onShowDelayElapsed();
		Eina_Bool onShowMinElapsed();

		Size m_Size;
		Evas_Object *m_Layout;

		Ecore_Timer *m_ShowDelayTimer;
		Ecore_Timer *m_ShowMinTimer;
		bool m_IsDestroyPending;
	};
}

#endif /* UI_PROCESS_POPUP_H */
