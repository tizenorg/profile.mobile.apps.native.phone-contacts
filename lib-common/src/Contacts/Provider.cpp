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

#include "Contacts/Provider.h"

using namespace Contacts;
using namespace std::placeholders;

void Provider::setInsertCallback(InsertCallback callback)
{
	auto handle = DbChangeObserver::getInstance()->addCallback(
			std::bind(&Provider::onInserted, this, _1, _2));
	m_InsertCallback = { std::move(callback), std::move(handle) };
}

void Provider::unsetInsertCallback()
{
	if (m_InsertCallback.first) {
		auto handle = m_InsertCallback.second;
		DbChangeObserver::getInstance()->removeCallback(handle);
		m_InsertCallback.first = nullptr;
	}
}

void Provider::onInserted(int id, contacts_changed_e changeType)
{
	if (changeType == CONTACTS_CHANGE_INSERTED) {
		auto callback = m_InsertCallback.first;
		if (callback) {
			callback(createContactData(id));
		}
	}
}
