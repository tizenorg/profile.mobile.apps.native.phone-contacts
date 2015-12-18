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
			 * @remark It can be update or delete ot insert of contact
			 * @param[in]    id          Contact ID
			 * @param[in]    callback    Change contact callback
			 */
			void setChangeContactCallback(int id, ChangeContactCallback callback);

			/**
			 * @brief Unset contact change callback
			 * @param[in]    id    Contact ID
			 */
			void unsetChangeContactCallback(int id);

			/**
			 * @brief Set log change callback
			 * @remark It can be update or delete or insert of contact
			 * @param[in]    callback    Change Log callback
			 */
			void setChangeLogCallback(ChangeLogCallback callback);

			/**
			 * @brief Unset log change callback
			 */
			void unsetChangeLogCallback();

		private:
			void fillList();
			bool shouldGroupLog(LogPtr log, LogPtr prevLog);
			LogGroupPtr groupLogs(LogPtr log, LogPtr prevLog);
			void addCurrentLog(contacts_list_h *list);
			contacts_list_h fetchList();

			void onLogChanged(const char *viewUri);
			void onContactChanged(const char *viewUri);

			void notifyLogWithChange(int contactId, int changeType);

			LogList m_AllLogs;
			int m_DbVersion;

			ChangeContactCallback m_ContactCallback;
			std::multimap<int, ChangeContactCallback> m_ChangeContactCallbacks;

			ChangeLogCallback m_LogCallback;
		};
	}
}

#endif /* LOGS_MODEL_LOG_PROVIDER_H */
