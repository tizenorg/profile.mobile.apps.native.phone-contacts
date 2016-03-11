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

#ifndef CONTACTS_COMMON_CHOOSER_H
#define CONTACTS_COMMON_CHOOSER_H

#include "Contacts/Common/ContactSelectTypes.h"
#include "Ui/Naviframe.h"

namespace Contacts
{
	namespace Common
	{
		/**
		 * @brief Controller and navigator for selecting different types of results
		 *        via ListView and DetailsView.
		 */
		class EXPORT_API Chooser : public Ui::Naviframe
		{
		public:
			/**
			 * @brief Create chooser
			 * @param[in]   selectMode  Selection mode
			 * @param[in]   resultType  Requested result type
			 * @param[in]   selectLimit Selection limit for #SelectMulti mode
			 */
			Chooser(SelectMode selectMode, ResultType resultType, size_t selectLimit);

			/**
			 * @brief Set selection callback
			 * @param[in]   callback    Selection callback
			 */
			void setSelectCallback(SelectCallback callback);

		private:
			virtual void onCreated() override;
			virtual void onPageAttached(Ui::NavigatorPage *page) override;

			bool onSinglePersonSelected(SelectResults personResults);
			bool onMultiPersonSelected(SelectResults personResults);
			bool onSelected(SelectResults results);

			static int getSingleResultId(int personId, ResultType resultType);
			static int getFilterType(ResultType resultType);

			SelectMode m_SelectMode;
			ResultType m_ResultType;

			int m_FilterType;
			size_t m_SelectLimit;

			SelectCallback m_OnSelected;
		};
	}
}

#endif /* CONTACTS_COMMON_CHOOSER_H */
