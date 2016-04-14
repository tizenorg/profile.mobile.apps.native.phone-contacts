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
#include "Ui/ListPopup.h"

namespace Phone
{
	namespace Dialer
	{
		class KeypadEntry;

		/**
		 * @brief Predictive search result list popup.
		 */
		class SearchResultsPopup : public Ui::ListPopup
		{
		public:
			/**
			 * @brief Constructor
			 * @param[in]   result  Search results
			 */
			SearchResultsPopup(const SearchResults *results);

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onCreated() override;

			virtual char *getItemText(void *data, const char *part) override;
			virtual Evas_Object *getItemContent(void *data, const char *part) override;

			const SearchResults *m_Results;
		};
	}
}

#endif /* PHONE_DIALER_SEARCH_RESULTS_POPUP_H */
