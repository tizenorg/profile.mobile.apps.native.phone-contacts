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

#ifndef UI_NAVIFRAME_H
#define UI_NAVIFRAME_H

#include "Ui/Navigator.h"
#include "Ui/NaviframePage.h"

namespace Ui
{
	/**
	 * @brief Naviframe wrapper
	 */
	class EXPORT_API Naviframe : public Navigator
	{
	public:
		Naviframe();

		/**
		 * @see Navigator::getCurrentPage()
		 */
		virtual NaviframePage *getCurrentPage() const override;

	protected:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;
		virtual NaviframePage *attachView(View *view) override;
		virtual void navigateToPage(NavigatorPage *page) override;
		virtual void navigateFromPage(NavigatorPage *page) override;

		Eina_Bool onItemPop(Elm_Object_Item *item);
		void onTransition(Evas_Object *obj, void *eventInfo);
		virtual bool onBackPressed() override;
	};
}

#endif /* UI_NAVIFRAME_H */
