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

#ifndef PHONE_DIALER_SEARCH_RESULTS_POPUP_H
#define PHONE_DIALER_SEARCH_RESULTS_POPUP_H

#include "Phone/Dialer/SearchTypes.h"
#include "Ui/Popup.h"

namespace Phone
{
	namespace Dialer
	{
		class KeypadEntry;

		/**
		 * @brief Predictive search result list popup.
		 */
		class SearchResultsPopup : public Ui::Popup
		{
		public:
			/**
			 * @brief Result selection callback
			 * @param[in]   Selected result
			 */
			typedef std::function<void(SearchResultPtr)> SelectedCallback;

			/**
			 * @brief Constructor
			 * @param[in]   result  Search results
			 */
			SearchResultsPopup(const SearchResults *results);

			/**
			 * @brief Set result selected callback
			 */
			void setSelectedCallback(SelectedCallback callback);

		private:
			virtual void onCreated() override;
			Evas_Object *createContactList(Evas_Object *parent);
			Elm_Genlist_Item_Class *createItemClass();

			static char *getItemText(void *data, Evas_Object *obj, const char *part);
			static Evas_Object *getItemContent(void *data, Evas_Object *obj, const char *part);
			void onItemSelected(Evas_Object *obj, void *event_info);

			const SearchResults *m_Results;
			SelectedCallback m_OnSelected;
		};
	}
}

#endif /* PHONE_DIALER_SEARCH_RESULTS_POPUP_H */
