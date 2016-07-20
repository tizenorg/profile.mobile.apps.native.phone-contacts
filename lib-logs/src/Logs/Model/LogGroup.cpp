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

#include "Logs/Model/LogGroup.h"
#include "Logs/Model/Log.h"
#include "Utils/Logger.h"

using namespace Logs::Model;

LogGroup::LogGroup(Log *log)
	: m_ChangedType(ChangeNone)
{
	addLog(log);
}

const LogGroup::LogList &LogGroup::getLogList() const
{
	return m_LogList;
}

void LogGroup::updateLogList()
{
	for (auto &&log : m_LogList) {
		log->updateContactRecord();
	}
}

void LogGroup::mergeLogGroup(LogGroup &group)
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
	if (m_OnLogAdded) {
		m_OnLogAdded(log);
	}
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

LogGroup::ChangeCbHandle LogGroup::addChangeCallback(ChangeCallback callback)
{
	m_ChangeCbs.push_back(std::move(callback));
	return --m_ChangeCbs.cend();
}

void LogGroup::removeChangeCallback(ChangeCbHandle handle)
{
	m_ChangeCbs.erase(handle);
}

void LogGroup::onChange()
{
	if (!m_ChangeCbs.empty()) {
		if (m_ChangedType != ChangeNone) {
			if (m_LogList.empty()) {
				m_ChangedType = ChangeRemoved;
			}
			for (auto &changeCb : m_ChangeCbs) {
				changeCb(m_ChangedType);
			}
			m_ChangedType = ChangeNone;
		}
	}
}

void LogGroup::setChangedType(int type)
{
	if (!m_ChangeCbs.empty()) {
		m_ChangedType |= type;
	}
}

void LogGroup::setLogAddCallback(LogAddCallback callback)
{
	m_OnLogAdded = std::move(callback);
}

void LogGroup::unsetLogAddCallback()
{
	m_OnLogAdded = nullptr;
}
