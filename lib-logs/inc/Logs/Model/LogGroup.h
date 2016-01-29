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

#include <list>
#include <functional>

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
			/**
			 * @brief Changed log callback
			 */
			typedef std::function<void()> ChangeCallback;

			/**
			 * @brief List of logs.
			 */
			typedef std::list<Log *> LogList;

			/**
			 * @brief Constructor
			 * @param[in]   log  Log to add
			 */
			LogGroup(Log *log);

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
			void setChangeCallback(ChangeCallback callback);

			/**
			 * @brief Unset log change callback
			 */
			void unsetChangeCallback();

			/**
			 * @brief Call log change callback
			 */
			void onChange();

			/**
			 * @brief Add log to LogGroup
			 * @param[in]   log  Log to add
			 */
			void addLog(Log *log);

			/**
			 * @brief Remove log from log group
			 * @param[in]   log  Log to remove
			 */
			void removeLog(Log *log);

		private:
			LogList m_LogList;

			ChangeCallback m_LogChangeCallback;
		};
	}
}
#endif /* LOGS_MODEL_LOG_GROUP_H */

