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

#ifndef MODEL_DATA_PROVIDER_H
#define MODEL_DATA_PROVIDER_H

#include "Model/DataItem.h"

#include <list>

namespace Model
{
	class EXPORT_API DataProvider
	{
	public:
		typedef std::list<DataItem *> DataList;

		/**
		 * @brief Insert callback
		 * @param[in] data
		 */
		typedef std::function<void(DataItem &)> InsertCallback;

		virtual ~DataProvider() { }

		/**
		 * @return Data list
		 */
		virtual const DataList &getDataList() = 0;

		/**
		 * @brief Clear data list
		 */
		virtual void clearDataList() { }

		/**
		 * @brief Set insert callback
		 * @param[in]   callback    Create callback
		 */
		void setInsertCallback(InsertCallback callback);

		/**
		 * @brief Unset insert callback
		 */
		void unsetInsertCallback();

	protected:
		/**
		 * @brief DataItem inserted callback
		 * @param[in]   dataItem    Data item
		 */
		void onInserted(DataItem &dataItem);

	private:
		InsertCallback m_OnInserted;
	};
}



#endif /* MODEL_DATA_PROVIDER_H */
