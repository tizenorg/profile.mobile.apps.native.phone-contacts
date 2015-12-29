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
		 * @brief Smart pointer to Log.
		 */
		typedef std::shared_ptr<Log> LogPtr;

		/**
		 * @brief List of logs.
		 */
		typedef std::list<LogPtr> LogList;

		/**
		 * @brief Smart pointer of LogGroup.
		 */
		typedef std::shared_ptr<LogGroup> LogGroupPtr;

		/**
		 * @brief Changed log callback
		 */
		typedef std::function<void()> LogChangeCallback;

		/**
		 * @brief Log remove callback
		 */
		typedef std::function<void()> LogRemoveCallback;

		/**
		 * @brief New log list callback
		 * @param[in]    list    Log list
		 */
		typedef std::function<void(LogList list)> NewLogListCallback;
	}
}

#endif /* LOGS_MODEL_LOG_TYPE_H */
