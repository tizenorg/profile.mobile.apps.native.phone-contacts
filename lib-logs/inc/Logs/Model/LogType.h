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

#ifndef LOGS_MODEL_LOG_TYPE_H
#define LOGS_MODEL_LOG_TYPE_H

#include <list>
#include <set>
#include <memory>
#include <functional>
#include <contacts.h>

namespace Logs
{
	namespace Model
	{
		class Log;
		class LogGroup;

		/**
		 * @brief List of logs.
		 */
		typedef std::list<Log *> LogList;

		/**
		 * @brief Log iterator.
		 */
		typedef LogList::iterator logIterator;

		/**
		 * @brief List of log groups.
		 */
		typedef std::list<LogGroup *> LogGroupList;

		/**
		 * @brief sET of log groups.
		 */
		typedef std::set<LogGroup *> LogGroupSet;

		/**
		 * @brief Changed log callback
		 */
		typedef std::function<void()> LogChangeCallback;

		/**
		 * @brief Log remove callback
		 */
		typedef std::function<void()> LogRemoveCallback;

		/**
		 * @brief New log group callback
		 * @param[in]    logGroup    Log group
		 */
		typedef std::function<void(LogGroupList groupList)> NewLogGroupCallback;
	}
}

#endif /* LOGS_MODEL_LOG_TYPE_H */
