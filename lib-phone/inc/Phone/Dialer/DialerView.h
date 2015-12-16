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

#ifndef PHONE_DIALER_DIALER_VIEW_H
#define PHONE_DIALER_DIALER_VIEW_H

#include "App/AppControl.h"
#include "Phone/Dialer/SearchEngine.h"
#include "Ui/View.h"
#include <string>

namespace Ui
{
	class Button;
}

namespace Phone
{
	namespace Dialer
	{
		class KeypadEntry;
		class SearchResultsControl;

		/**
		 * @brief Dialer main view.
		 */
		class EXPORT_API DialerView : public Ui::View
		{
		public:
			DialerView();
			virtual ~DialerView() override;

			/**
			 * @brief Set number to be displayed
			 * @param[in]   number  Number
			 */
			void setNumber(const std::string &number);

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onCreated() override;
			virtual void onPageAttached() override;
			virtual void onNavigation(bool isCurrentView) override;
			virtual void onMenuPressed() override;

			Evas_Object *createEntry(Evas_Object *parent);
			Evas_Object *createSearchControl(Evas_Object *parent);
			Evas_Object *createKeypad(Evas_Object *parent);
			Evas_Object *createCallButton(Evas_Object *parent);
			Evas_Object *createBackspaceButton(Evas_Object *parent);

			void onEntryChanged();
			void onResultSelected(SearchResultPtr result);

			void onKeyPressed(Ui::Button &button);
			bool onKeyLongpressed(Ui::Button &button);

			void onBackspacePressed(Ui::Button &button);
			bool onBackspaceLongpressed(Ui::Button &button);
			void onCallPressed(Evas_Object *obj, void *event_info);

			void onDbChanged(const char *uri);

			static void launchCall(const std::string &number);
			void launchSpeedDial(int digit);

			App::AppControl m_AppControl;
			SearchEngine m_SearchEngine;

			KeypadEntry *m_Entry;
			SearchResultsControl *m_SearchControl;
		};
	}
}

#endif /* PHONE_DIALER_DIALER_VIEW_H */
