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

#ifndef PHONE_DIALER_MAIN_VIEW_H
#define PHONE_DIALER_MAIN_VIEW_H

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
		class SearchResultsWidget;

		/**
		 * @brief Dialer main view.
		 */
		class EXPORT_API MainView : public Ui::View
		{
		public:
			MainView();
			virtual ~MainView() override;

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
			virtual Evas_Object *onMenuPressed() override;

			Evas_Object *createEntry(Evas_Object *parent);
			Evas_Object *createSearchWidget(Evas_Object *parent);
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
			void launchSpeeddial(int digit);
			static std::string getSpeeddialNumber(int digit);
			static std::string getLastNumber();

			App::AppControl m_AppControl;
			SearchEngine m_SearchEngine;

			KeypadEntry *m_Entry;
			SearchResultsWidget *m_SearchWidget;
		};
	}
}

#endif /* PHONE_DIALER_MAIN_VIEW_H */
