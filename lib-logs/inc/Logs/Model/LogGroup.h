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

#ifndef LOGS_MODEL_LOG_GROUP_H
#define LOGS_MODEL_LOG_GROUP_H

#include "Logs/Model/Log.h"
#include "Logs/Model/LogType.h"
#include <string>

namespace Logs
{
	namespace Model
	{
		class Log;
		/**
		 * @brief Group of logs
		 */
		class LogGroup
		{
		public:

			LogGroup(Log *log);

			~LogGroup();

			/**
			 * @brief Add log to LogGroup
			 * @param[in]   log  Log to add
			 */
			void addLog(Log *log);

			/**
			 * @brief Get log group list
			 * @return list of log. The reference will be valid till this LogGroup object exist.
			 */
			const LogList &getLogList() const;

			/**
			 * @brief Set log changed callback
			 * @remark Callback called when log is changed.
			 * @param[in]    callback    Changed log callback
			 */
			void setLogChangeCallback(LogChangeCallback callback);

			/**
			 * @brief Unset log change callback
			 */
			void unsetLogChangeCallback();

			/**
			 * @brief Call log change callback
			 */
			void callLogChangeCallback();

			/**
			 * @brief Remove log from log group
			 * @param[in]   log  Log to remove
			 */
			void removeLog(Log *log);

			/**
			 * @param[in]   personId  New person id
			 * @return true if log person id changed
			 */
			bool changedPersonId(int personId);

			/**
			 * @return first log in list;
			 */
			Log *getFirstLog();

		private:
			LogList m_LogList;

			LogChangeCallback m_LogChangeCallback;
		};
	}
}
#endif /* LOGS_MODEL_LOG_GROUP_H */

