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
			 * @brief Determines how to filter log list
			 */
			enum FilterType
			{
				FilterNone ,
				FilterAll,
				FilterMissed
			};

			/**
			 * @brief Constructor
			 */
			LogProvider(LogProvider::FilterType filterType);

			/**
			 * @brief Destructor
			 */
			~LogProvider();

			/**
			 * @brief Get log list
			 * @return list of logs.
			 */
			LogList getLogList();

			/**
			 * @brief Set log change callback
			 * @remark It can be update or delete or insert of contact
			 * @param[in]    callback    Change Log callback
			 */
			void setLogChangeCallback(LogChangeCallback callback);

			/**
			 * @brief Unset log change callback
			 */
			void unsetLogChangeCallback();

		private:
			void fillList(LogList &logList);
			bool shouldGroupLogs(LogPtr log, LogPtr prevLog);
			LogGroupPtr groupLogs(LogPtr log, LogPtr prevLog);

			void addLog(LogList &logList, contacts_record_h record);
			void addFirstLog(LogList &logList, contacts_record_h record, contacts_list_h list);

			contacts_filter_h getProviderFilter(LogProvider::FilterType filterType);
			contacts_list_h fetchLogList();

			void onLogChanged(const char *viewUri);
			void onContactChanged(const char *viewUri);

			void notifyLogWithChange(int contactId, contacts_changed_e changeType);

			FilterType m_ListFilterType;
			int m_DbVersion;

			std::multimap<int, LogPtr> m_Logs;

			LogChangeCallback m_LogCallback;
		};
	}
}

#endif /* LOGS_MODEL_LOG_PROVIDER_H */
