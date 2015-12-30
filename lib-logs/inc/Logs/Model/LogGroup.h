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
		/**
		 * @brief Group of logs
		 */
		class LogGroup
		{
		public:

			LogGroup(LogPtr log);

			~LogGroup();

			/**
			 * @brief Add log to LogGroup
			 * @param[in]   log  Log to add
			 */
			void addLog(LogPtr log);

			/**
			 * @brief Get log group list
			 * @return list of log. The reference will be valid till this LogGroup object exist.
			 */
			const LogList &getLogList() const;

			/**
			 * @brief Remove deleted logs from list
			 * @param[in]   logGroup  Changed log list
			 * @return count of removed logs from list  .
			 */
			int removeDeletedLogs(const LogGroupPtr &logGroup);

			/**
			 * @brief Add new logs in list
			 * @param[in]   logGroup  Changed log list
			 * @return count of add logs in list.
			 */
			int addNewLogs(const LogGroupPtr &logGroup);

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
			 * @brief Set log remove callback
			 * @remark Callback called when log removed.
			 * @param[in]    callback    Removed log callback
			 */
			void setLogRemoveCallback(LogRemoveCallback callback);

			/**
			 * @brief Unset log remove  callback
			 */
			void unsetLogRemoveCallback();

			/**
			 * @brief Call log remove callback
			 */
			void callLogRemoveCallback();

			/**
			 * @param[in]   personId  New person id
			 * @return true if log person id changed
			 */
			bool changedPersonId(int personId);

			/**
			 * @return true if log name changed
			 */
			bool changedName();

			/**
			 * @return first log in list;
			 */
			LogPtr getFirstLog();

			/**
			 * @brief Update group.
			 */
			void updateGroup();

		private:
			LogList m_LogList;

			LogChangeCallback m_LogChangeCallback;
			LogRemoveCallback m_LogRemoveCallback;

			int m_PersonId;
			std::string m_LogName;
		};
	}
}
#endif /* LOGS_MODEL_LOG_GROUP_H */

