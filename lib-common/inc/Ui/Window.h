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

#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include "Ui/Control.h"

namespace Ui
{
	class View;

	/**
	 * @brief Application main window
	 */
	class EXPORT_API Window : public Control
	{
	public:
		Window();

		/**
		 * @return Conformant Evas_Object.
		 */
		Evas_Object *getConformant() const;

		/**
		 * @return Basic Window layout Evas_Object.
		 */
		Evas_Object *getBaseLayout() const;

		/**
		 * @brief Attach main View to be displayed in the Window.
		 * @param[in]   view    Window main View
		 */
		void attachView(View *view);

	private:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;
		void onBackPressed(Evas_Object *obj, void *eventInfo);
		void onMenuPressed(Evas_Object *obj, void *eventInfo);

		Evas_Object *m_Conform;
		Evas_Object *m_Layout;
		View *m_MainView;
	};
}

#endif /* UI_WINDOW_H */
