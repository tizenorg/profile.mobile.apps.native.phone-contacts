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

#include "Model2/DataProvider.h"

using namespace Model2;

DataProvider::DataProvider()
	: m_IsInitialized(false),
	  m_IsUpdateEnabled(true), m_IsUpdatePending(false), m_IsUpdating(false)
{
}

const DataProvider::DataList &DataProvider::getDataList()
{
	return m_DataList;
}

void DataProvider::initialize(InitializeCallback callback)
{
	if (m_IsInitialized) {
		return;
	}

	m_OnInitialized = std::move(callback);
	onInitialize();
}

void DataProvider::update()
{
	if (!m_IsInitialized) {
		return;
	}

	if (!m_IsUpdateEnabled || m_IsUpdating) {
		m_IsUpdatePending = true;
		return;
	}

	m_IsUpdatePending = false;
	onUpdate();
}

void DataProvider::setUpdateMode(bool isEnabled)
{
	m_IsUpdateEnabled = isEnabled;
	if (m_IsUpdateEnabled && m_IsUpdatePending) {
		update();
	}
}

void DataProvider::setInsertCallback(InsertCallback callback)
{
	m_OnInserted = std::move(callback);
}

void DataProvider::setUpdateStartCallback(UpdateCallback callback)
{
	m_OnUpdateStarted = std::move(callback);
}

void DataProvider::setUpdateFinishCallback(UpdateCallback callback)
{
	m_OnUpdateFinished = std::move(callback);
}

void DataProvider::onInitialize()
{
	onInitialized({ });
}

void DataProvider::onUpdate()
{
	onUpdateStarted();
	onUpdateFinished();
}

void DataProvider::insertDataItem(DataItemPtr dataItem)
{
	dataItem->m_ChangeType = ChangeInsert;
	m_DataList.push_back(std::move(dataItem));
}

void DataProvider::deleteDataItem(DataItem &dataItem)
{
	dataItem.m_ChangeType = ChangeDelete;
}

void DataProvider::onInitialized(DataList dataList)
{
	m_DataList = std::move(dataList);
	m_IsInitialized = true;

	if (m_OnInitialized) {
		m_OnInitialized();
	}
}

void DataProvider::onUpdateStarted()
{
	m_IsUpdating = true;
	if (m_OnUpdateStarted) {
		m_OnUpdateStarted();
	}
}

void DataProvider::onUpdateFinished()
{
	for (auto it = m_DataList.begin(); it != m_DataList.end(); ) {
		auto changeType = (*it)->m_ChangeType;
		(*it)->onUpdated();

		if (changeType == ChangeInsert) {
			if (m_OnInserted) {
				m_OnInserted(**it);
			}
		} else if (changeType == ChangeDelete) {
			it = m_DataList.erase(it);
			continue;
		}

		++it;
	}

	if (m_OnUpdateFinished) {
		m_OnUpdateFinished();
	}

	m_IsUpdating = false;
	if (m_IsUpdatePending) {
		update();
	}
}
