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

#include "Contacts/ContactData.h"

using namespace Contacts;

ContactData::ContactData(Type type)
	: m_Type(type)
{
}

ContactData::Type ContactData::getType() const
{
	return m_Type;
}

void ContactData::setUpdateCallback(UpdateCallback callback)
{
	m_OnUpdated = std::move(callback);
}

void ContactData::unsetUpdateCallback()
{
	m_OnUpdated = nullptr;
}

void ContactData::setDeleteCallback(DeleteCallback callback)
{
	m_OnDeleted = std::move(callback);
}

void ContactData::unsetDeleteCallback()
{
	m_OnDeleted = nullptr;
}

void ContactData::onUpdated(int changes)
{
	if (m_OnUpdated) {
		m_OnUpdated(changes);
	}
}

void ContactData::onDeleted()
{
	if (m_OnDeleted) {
		m_OnDeleted();
	}
}
