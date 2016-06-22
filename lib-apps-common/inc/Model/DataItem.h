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

#ifndef MODEL_DATA_ITEM_H
#define MODEL_DATA_ITEM_H

#include <functional>
#include <tizen.h>

namespace Model
{
	class EXPORT_API DataItem
	{
	public:
		/**
		 * @brief Change callback
		 * @param[in] Changed info
		 */
		typedef std::function<void(int)> UpdateCallback;

		/**
		 * @brief Delete callback
		 */
		typedef std::function<void()> DeleteCallback;

		virtual ~DataItem() { }

		/**
		 * @brief Set update callback
		 * @param[in]   callback    Update callback
		 */
		void setUpdateCallback(UpdateCallback callback);

		/**
		 * @brief Unset update callback
		 */
		void unsetUpdateCallback();

		/**
		 * @brief Set delete callback
		 * @param[in]   callback    Delete callback
		 */
		void setDeleteCallback(DeleteCallback callback);

		/**
		 * @brief Unset delete callback
		 */
		void unsetDeleteCallback();

	protected:
		/**
		 * @brief ContactData updated callback
		 * @param[in]   changes     Changed info
		 */
		void onUpdated(int changes);

		/**
		 * @brief ContactData deleted callback
		 */
		void onDeleted();

	private:
		UpdateCallback m_OnUpdated;
		DeleteCallback m_OnDeleted;
	};
}



#endif /* MODEL_DATA_ITEM_H */
