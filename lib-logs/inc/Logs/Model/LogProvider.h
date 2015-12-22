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

#ifndef LOGS_MODEL_LOG_PROVIDER_H
#define LOGS_MODEL_LOG_PROVIDER_H

#include <map>
#include <contacts.h>
#include "Logs/Model/LogType.h"

namespace Logs
{
	namespace Model
	{
		/**
		 * @brief Provides list of logs
		 */
		class LogProvider
		{
		public:
			/**
			 * @brief Constructor
			 */
			LogProvider();

			/**
			 * @brief Destructor
			 */
			~LogProvider();

			/**
			 * @brief Get log list
			 * @return list of logs.
			 */
			const LogList &getLogList() const;

			/**
			 * @brief Set contact change callback
			 * @remark Callback called when contact update or delete or insert
			 * @param[in]    id          Contact ID
			 * @param[in]    callback    Change contact callback
			 */
			void addChangeContactCallback(int id, ContactChangeCallback callback);

			/**
			 * @brief Unset contact change callback
			 * @param[in]    id    Contact ID
			 */
			void removeChangeContactCallback(int id);

			/**
			 * @brief Set log change callback
			 * @remark It can be update or delete or insert of contact
			 * @param[in]    callback    Change Log callback
			 */
			void setChangeLogCallback(LogChangeCallback callback);

			/**
			 * @brief Unset log change callback
			 */
			void unsetChangeLogCallback();

		private:
			void fillList();
			bool shouldGroupLogs(LogPtr log, LogPtr prevLog);
			bool isTimeEqual(struct tm logTime, struct tm prevLogTime);
			LogGroupPtr groupLogs(LogPtr log, LogPtr prevLog);
			void addLog(contacts_record_h record);
			void addFirstLog(contacts_record_h record);
			contacts_list_h fetchLogList();

			void onLogChanged(const char *viewUri);
			void onContactChanged(const char *viewUri);

			void notifyLogWithChange(int contactId, contacts_changed_e changeType);

			LogList m_AllLogs;
			int m_DbVersion;

			std::multimap<int, ContactChangeCallback> m_ChangeContactCallbacks;

			LogChangeCallback m_LogCallback;
		};
	}
}

#endif /* LOGS_MODEL_LOG_PROVIDER_H */
