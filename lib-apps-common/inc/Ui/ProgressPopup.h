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

#ifndef UI_PROGRESS_POPUP_H
#define UI_PROGRESS_POPUP_H

#include "Ui/Popup.h"

namespace Ui
{
	class EXPORT_API ProgressPopup : public Popup
	{
	public:
		/**
		 * @brief Create progress popup.
		 * @param[in]   maxValue    Max value of progressbar
		 * @param[in]   isVisible   Popup visibility on start
		 */
		ProgressPopup(size_t maxValue, bool isVisible = true);

		/**
		 * @brief Set progress of the progressbar
		 * @param[in]   value   Progress value
		 */
		void setProgress(size_t value);

	private:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;

	private:
		size_t m_MaxValue;
		Evas_Object *m_Progressbar;
	};
}

#endif /* UI_PROGRESS_POPUP_H */
