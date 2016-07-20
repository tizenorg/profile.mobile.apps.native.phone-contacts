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

#ifndef PHONE_DIALER_SEARCH_RESULTS_CONTROL_H
#define PHONE_DIALER_SEARCH_RESULTS_CONTROL_H

#include "Ui/Control.h"
#include "Phone/Dialer/SearchTypes.h"

namespace Phone
{
	namespace Dialer
	{
		/**
		 * @brief Predictive search result widget.
		 */
		class SearchResultsControl : public Ui::Control
		{
		public:
			/**
			 * @brief Result selection callback
			 * @param[in]   Selected result
			 */
			typedef std::function<void(SearchResultPtr)> SelectedCallback;

			SearchResultsControl();

			/**
			 * @brief Set search results to be displayed
			 * @param[in]   results     Search results
			 */
			void setResults(const SearchResults *results);

			/**
			 * @brief Clear search results and display nothing
			 */
			void clearResults();

			/**
			 * @brief Set result selected callback
			 */
			void setSelectedCallback(SelectedCallback callback);

		private:
			enum ResultsState
			{
				ResultsNone,
				ResultsEmpty,
				ResultsPresent,
				ResultsMany
			};

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;

			void setLayout(const char *groupName);
			void clearLayout();

			void setResultsEmpty();
			void setResultsPresent();
			void setResultsCount(size_t count);

			void setResultInfo(SearchResultPtr result);
			void setResultSpeedDial(SearchResultPtr result);

			void onResultPressed();
			void onShowResultsPressed();

			const SearchResults *m_Results;
			ResultsState m_State;
			SelectedCallback m_OnSelected;
			Evas_Object *m_ResultsCount;
		};
	}
}

#endif /* PHONE_DIALER_SEARCH_RESULTS_CONTROL_H */
