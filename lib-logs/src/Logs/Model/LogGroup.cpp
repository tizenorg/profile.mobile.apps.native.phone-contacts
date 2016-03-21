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
#include "Utils/Logger.h"

using namespace Logs::Model;

LogGroup::LogGroup(Log *log)
{
	addLog(log);
}

LogGroup::LogList &LogGroup::getLogList()
{
	return m_LogList;
}

void LogGroup::addLogList(LogList &list)
{
	m_LogList.merge(list);
}

void LogGroup::setChangeCallback(ChangeCallback callback)
{
	m_ChangeCallback = std::move(callback);
}

void LogGroup::unsetChangeCallback()
{
	m_ChangeCallback = nullptr;
}

void LogGroup::onChange()
{
	if (m_ChangeCallback) {
		m_ChangeCallback(m_ChangedType);
	}
}

void LogGroup::addLog(Log *log)
{
	m_ChangedType |= LogGroup::UpdateCount;
	m_LogList.push_back(log);
	log->setLogGroup(this);
}

void LogGroup::removeLog(Log *log)
{
	m_ChangedType |= LogGroup::UpdateCount;
	m_LogList.remove(log);
	log->setLogGroup(nullptr);
}

void LogGroup::remove()
{
	for (auto &&log : m_LogList) {
		log->remove();
	}
}

Log &LogGroup::getFirstLog()
{
	return *(*m_LogList.begin());
}

void LogGroup::updateLogList()
{
	for(auto &&log : m_LogList) {
		log->update();
	}
}

void LogGroup::setChangedType(int type)
{
	m_ChangedType |= type;
}
