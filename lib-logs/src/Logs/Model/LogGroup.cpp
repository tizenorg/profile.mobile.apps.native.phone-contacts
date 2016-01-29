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

#include "Logs/Model/LogGroup.h"
#include "Logs/Model/Log.h"
#include <algorithm>
#include "Utils/Logger.h"
#include <cstring>

using namespace Logs::Model;

LogGroup::LogGroup(Log *log)
{
	addLog(log);
}

const LogGroup::LogList &LogGroup::getLogList() const
{
	return m_LogList;
}

void LogGroup::setChangeCallback(ChangeCallback callback)
{
	m_LogChangeCallback = std::move(callback);
}

void LogGroup::unsetChangeCallback()
{
	m_LogChangeCallback = nullptr;
}

void LogGroup::onChange()
{
	if (m_LogChangeCallback) {
		m_LogChangeCallback();
	}
}

void LogGroup::removeLog(Log *log)
{
	m_LogList.remove(log);
	log->setLogGroup(nullptr);
}

void LogGroup::addLog(Log *log)
{
	m_LogList.push_back(log);
	log->setLogGroup(this);
}
