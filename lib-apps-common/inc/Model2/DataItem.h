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
#include <memory>
#include <tizen.h>

/* FIXME: Rename once transition to new model is complete */
namespace Model2
{
	/**
	 * @brief Change types.
	 */
	enum ChangeType
	{
		ChangeNone,
		ChangeInsert,
		ChangeUpdate,
		ChangeDelete
	};

	class EXPORT_API DataItem
	{
	public:
		/**
		 * @brief Called after item was updated.
		 * @param[in] Which item data was updated (depends on the specific item).
		 */
		typedef std::function<void(int)> UpdateCallback;

		/**
		 * @brief Called before item is deleted.
		 */
		typedef std::function<void()> DeleteCallback;

		DataItem();
		virtual ~DataItem() { }

		/**
		 * @brief Update item with new data.
		 * @param[in]   data    New item data.
		 */
		void update(void *data);

		/**
		 * @brief Set update callback.
		 * @param[in]   callback    Update callback
		 */
		void setUpdateCallback(UpdateCallback callback);

		/**
		 * @brief Set delete callback.
		 * @param[in]   callback    Delete callback
		 */
		void setDeleteCallback(DeleteCallback callback);

	protected:
		/**
		 * @brief Called after update() to update item data.
		 * @param[in]   data    New item data
		 * @return Mask specifying which item data was updated.
		 */
		virtual int onUpdate(void *data) { return 0; }

	private:
		friend class DataProvider;
		void onUpdated();

		int m_Changes;
		ChangeType m_ChangeType;

		UpdateCallback m_OnUpdated;
		DeleteCallback m_OnDeleted;
	};

	typedef std::unique_ptr<DataItem> DataItemPtr;
}

#endif /* MODEL_DATA_ITEM_H */
