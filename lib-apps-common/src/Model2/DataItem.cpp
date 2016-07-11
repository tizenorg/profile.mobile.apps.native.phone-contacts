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

#include "Model2/DataItem.h"

using namespace Model2;

DataItem::DataItem()
	: m_Changes(0), m_ChangeType(ChangeNone)
{
}

void DataItem::update(void *data)
{
	m_Changes = onUpdate(data);
	m_ChangeType = ChangeUpdate;
}

void DataItem::setUpdateCallback(UpdateCallback callback)
{
	m_OnUpdated = std::move(callback);
}

void DataItem::setDeleteCallback(DeleteCallback callback)
{
	m_OnDeleted = std::move(callback);
}

void DataItem::onUpdated()
{
	switch (m_ChangeType) {
		case ChangeUpdate:
			if (m_OnUpdated && m_Changes) {
				m_OnUpdated(m_Changes);
			}
			break;
		case ChangeDelete:
			if (m_OnDeleted) {
				m_OnDeleted();
			}
			break;
		default:
			break;
	}

	m_Changes = 0;
	m_ChangeType = ChangeNone;
}
