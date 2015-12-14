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

using namespace Logs::Model;

bool LogGroup::isGroup() const
{
	return true;
}

const char *LogGroup::getName() const
{
	return m_LogList.back()->getName();
}

const char *LogGroup::getNumber() const
{
	return m_LogList.back()->getNumber();
}

const char *LogGroup::getImagePath() const
{
	return m_LogList.back()->getImagePath();
}

int LogGroup::getType() const
{
	return m_LogList.back()->getType();
}

int LogGroup::getTime() const
{
	return m_LogList.back()->getTime();
}

int LogGroup::getId() const
{
	return 0;
}

void LogGroup::removeLog(int id)
{
	auto position = std::find_if(m_LogList.begin(), m_LogList.end(), [id](LogPtr log) {
		return (log->getId() == id);
	});
	m_LogList.erase(position);
}

void LogGroup::addLog(LogPtr log)
{
	m_LogList.push_back(log);
}

const LogList &LogGroup::getLogList() const
{
	return m_LogList;
}
