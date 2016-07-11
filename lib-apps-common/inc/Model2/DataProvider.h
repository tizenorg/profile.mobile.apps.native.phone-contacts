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

#include "Model2/DataItem.h"

#include <list>

/* FIXME: Rename once transition to new model is complete */
namespace Model2
{
	class EXPORT_API DataProvider
	{
	public:
		typedef std::list<DataItemPtr> DataList;

		/**
		 * @brief Called once initialization is finished.
		 */
		typedef std::function<void()> InitializeCallback;

		/**
		 * @brief Called after item was inserted.
		 * @param[in]   Inserted item
		 */
		typedef std::function<void(DataItem &)> InsertCallback;

		/**
		 * @brief Called when provider has started or finished updating its items.
		 */
		typedef std::function<void()> UpdateCallback;

		DataProvider();
		virtual ~DataProvider() { }

		/**
		 * @return Provider data list.
		 */
		const DataList &getDataList();

		/**
		 * @brief Initialize provider data.
		 * @remark Should be called before retrieving the data.
		 * @param[in]   callback    Initialization finished callback
		 */
		void initialize(InitializeCallback callback);

		/**
		 * @brief Update provider data.
		 * @details If update is disabled or is already in progress
		 *          the update is postponed.
		 */
		void update();

		/**
		 * @brief Set provider update mode.
		 * @param[in]   isEnabled   Whether data update is enabled.
		 */
		void setUpdateMode(bool isEnabled);

		/**
		 * @brief Set insert callback.
		 * @param[in]   callback    Create callback
		 */
		void setInsertCallback(InsertCallback callback);

		/**
		 * @brief Set update start callback.
		 * @param[in]   callback    Update start callback
		 */
		void setUpdateStartCallback(UpdateCallback callback);

		/**
		 * @brief Set update finish callback.
		 * @param[in]   callback    Update finish callback
		 */
		void setUpdateFinishCallback(UpdateCallback callback);

	protected:
		/**
		 * @brief Called after initialize() to initialize provider data.
		 * @post onInitialized() should be called when initialization is finished.
		 */
		virtual void onInitialize();

		/**
		 * @brief Called after update() to update provider data.
		 * @post onUpdateStarted() should be called when update has started.
		 * @post onUpdateFinished() should be called when update is finished.
		 */
		virtual void onUpdate();

		/**
		 * @brief Insert new DataItem into the list.
		 * @param[in]   dataItem    New data item
		 */
		void insertDataItem(DataItemPtr dataItem);

		/**
		 * @brief Delete DataItem from the list.
		 * @param[in]   dataItem    Data item to be deleted
		 */
		void deleteDataItem(DataItem &dataItem);

		/**
		 * @brief Should be called to notify that initialization is finished.
		 * @param[in]   dataList    Initialized data list
		 */
		void onInitialized(DataList dataList);

		/**
		 * @brief Should be called to notify that update has started.
		 */
		void onUpdateStarted();

		/**
		 * @brief Should be called to notify that update is finished.
		 * @details Notifies clients about changes and removes deleted items from the list.
		 */
		void onUpdateFinished();

	private:
		bool m_IsInitialized;
		bool m_IsUpdateEnabled;
		bool m_IsUpdatePending;
		bool m_IsUpdating;

		InitializeCallback m_OnInitialized;
		InsertCallback m_OnInserted;
		UpdateCallback m_OnUpdateStarted;
		UpdateCallback m_OnUpdateFinished;

		DataList m_DataList;
	};
}

#endif /* MODEL_DATA_PROVIDER_H */
