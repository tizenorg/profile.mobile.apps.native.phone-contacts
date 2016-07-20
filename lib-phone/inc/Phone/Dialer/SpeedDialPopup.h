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

#ifndef PHONE_DIALER_SPEEDDIAL_POPUP_H
#define PHONE_DIALER_SPEEDDIAL_POPUP_H

#include "Ui/Popup.h"
#include <app_control.h>

namespace Phone
{
	namespace Dialer
	{
		/**
		 * @brief "Add to speed dial" prompt popup.
		 */
		class SpeedDialPopup : public Ui::Popup
		{
		public:
			/**
			 * @brief Create speed dial popup.
			 * @param[in]   speedNumber Dialer key value
			 */
			SpeedDialPopup(int speedNumber);

		private:
			virtual void onCreated() override;
			bool onOkPressed();

			static void onPickResult(app_control_h request, app_control_h reply,
					app_control_result_e result, void *data);

			int m_SpeedNumber;
		};
	}
}

#endif /* PHONE_DIALER_SPEEDDIAL_POPUP_H */
