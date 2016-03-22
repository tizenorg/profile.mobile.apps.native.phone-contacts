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
	: m_ChangedType(ChangeNone)
{
	addLog(log);
}

LogGroup::LogList &LogGroup::getLogList()
{
	return m_LogList;
}

void LogGroup::updateLogList()
{
	for(auto &&log : m_LogList) {
		log->updateContact();
	}
}

void LogGroup::addLogList(LogGroup &group)
{
	group.setChangedType(ChangeRemoved);
	setChangedType(ChangeCount | ChangeTime);
	m_LogList.merge(group.m_LogList);
}

void LogGroup::addLog(Log *log)
{
	setChangedType(ChangeCount);
	m_LogList.push_back(log);
	log->setLogGroup(this);
}

void LogGroup::removeLog(Log *log)
{
	setChangedType(ChangeCount);
	m_LogList.remove(log);
	log->setLogGroup(nullptr);
}

Log &LogGroup::getFirstLog()
{
	return *(*m_LogList.begin());
}

void LogGroup::remove()
{
	for (auto &&log : m_LogList) {
		log->remove();
	}
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
		if (m_ChangedType != ChangeNone) {
			if (m_LogList.empty()) {
				m_ChangedType = ChangeRemoved;
			}

			m_ChangeCallback(m_ChangedType);
			m_ChangedType = ChangeNone;
		}
	}
}

void LogGroup::setChangedType(int type)
{
	if (m_ChangeCallback) {
		m_ChangedType |= type;
	}
}
