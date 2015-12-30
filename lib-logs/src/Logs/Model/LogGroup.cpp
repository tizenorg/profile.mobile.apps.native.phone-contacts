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
#include <algorithm>
#include <cstring>

using namespace Logs::Model;

LogGroup::LogGroup(LogPtr log)
{
	m_PersonId = log->getPersonId();
	m_LogName = log->getNumber();
	m_LogList.push_back(std::move(log));
}

LogGroup::~LogGroup()
{
}

void LogGroup::addLog(LogPtr log)
{
	m_LogList.push_back(log);
}

const LogList &LogGroup::getLogList() const
{
	return m_LogList;
}

int LogGroup::removeDeletedLogs(const LogGroupPtr &logGroup)
{
	auto changedIt = logGroup->getLogList().rbegin();
	auto oldIt = m_LogList.rbegin();

	int count = 0;

	while (logGroup->getLogList().rend() != oldIt) {
		if ((*changedIt)->getId() != (*oldIt)->getId()) {
			m_LogList.erase(oldIt.base());
			++count;
		} else {
			++changedIt;
		}
		++oldIt;
	}
	return count;
}

int LogGroup::addNewLogs(const LogGroupPtr &logGroup)
{
	auto changedIt = logGroup->getLogList().begin();
	auto oldIt = m_LogList.begin();

	int count = 0;

	while ((*changedIt)->getId() != (*oldIt)->getId()) {
		addLog(*changedIt);
		++changedIt;
		++count;
	}

	return count;
}

void LogGroup::setLogChangeCallback(LogChangeCallback callback)
{
	m_LogChangeCallback = std::move(callback);
}

void LogGroup::unsetLogChangeCallback()
{
	m_LogChangeCallback = nullptr;
}

void LogGroup::callLogChangeCallback()
{
	if (m_LogChangeCallback) {
		m_LogChangeCallback();
	}
}

void LogGroup::setLogRemoveCallback(LogRemoveCallback callback)
{
	m_LogRemoveCallback = std::move(callback);
}

void LogGroup::unsetLogRemoveCallback()
{
	m_LogRemoveCallback = nullptr;
}

void LogGroup::callLogRemoveCallback()
{
	if (m_LogRemoveCallback) {
		m_LogRemoveCallback();
	}
}

bool LogGroup::changedPersonId(int personId)
{
	LogPtr log = m_LogList.back();
	if (m_PersonId == personId) {
		if (m_PersonId != log->getPersonId()) {
			m_PersonId = log->getPersonId();
			return true;
		}
	} else {
		if (m_PersonId == log->getPersonId()) {
			m_PersonId = log->getPersonId();
			return true;
		}
	}
	return false;
}

bool LogGroup::changedName()
{
	LogPtr log = m_LogList.back();
	if (strcmp(m_LogName.c_str(), log->getName()) != 0) {
		m_LogName = log->getName();
		return true;
	}
	return false;
}

LogPtr LogGroup::getFirstLog()
{
	return m_LogList.back();
}

void LogGroup::updateGroup()
{
	for (auto &&it : m_LogList) {
		it->updateLog();
	}
}
