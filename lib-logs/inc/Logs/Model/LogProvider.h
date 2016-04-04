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

#include <contacts.h>
#include <time.h>
#include <memory>
#include <set>

#include "Contacts/Model/ContactRecordProvider.h"
#include "Logs/Model/Log.h"
#include "Logs/Model/LogGroup.h"

namespace Logs
{
	namespace Model
	{
		/**
		 * @brief Provides list of logs
		 */
		class LogProvider : public Contacts::Model::ContactDataProvider
		{
		public:
			/**
			 * @brief Unique log pointer.
			 */
			typedef std::unique_ptr<Log> LogPtr;

			/**
			 * @brief Unique log group pointer.
			 */
			typedef std::unique_ptr<LogGroup> LogGroupPtr;

			/**
			 * @brief Log list.
			 */
			typedef std::list<contacts_record_h> RecordList;

			/**
			 * @brief Unique log group list.
			 */
			typedef std::list<LogGroupPtr> LogGroupList;

			/**
			 * @brief Log group list.
			 */
			typedef std::list<LogGroup *> GroupList;

			/**
			 * @brief Log iterator.
			 */
			typedef Contacts::Model::ContactDataList::iterator LogIterator;

			/**
			 * @brief Record iterator.
			 */
			typedef RecordList::iterator RecordIterator;

			/**
			 * @brief Group iterator.
			 */
			typedef LogGroupList::iterator GroupIterator;

			/**
			 * @brief New log group callback
			 * @param[in]    logGroup    Log group
			 */
			typedef std::function<void(LogGroup *group)> InsertCallback;

			/**
			 * @brief Constructor
			 */
			LogProvider();

			/**
			 * @brief Destructor
			 */
			~LogProvider();

			/**
			 * @brief Get log group list
			 * @return list of logs groups.
			 */
			const LogGroupList &getLogGroupList();

			/**
			 * @brief Get log group list
			 * @return List of log group objects
			 */
			virtual const Contacts::Model::ContactDataList &getContactDataList() override;

			/**
			 * @brief Reset log group list
			 */
			void resetLogGroups();

			/**
			 * @brief Set new log callback
			 * @param[in]    callback    New Log callback
			 */
			void setInsertCallback(InsertCallback callback);

			/**
			 * @brief Unset new log callback
			 */
			void unsetInsertCallback();

			/**
			 * @brief Compare dates
			 * @param[in]    firstDate        First date
			 * @param[in]    secondDate       Second date
			 * @return true if dates are equal.
			 */
			static bool compareDate(const tm &firstDate, const tm &secondDate);

		private:
			void fillList();
			void fillRecordList(RecordList &recordList);
			size_t fillGroupList(LogIterator begin, LogIterator end);
			bool shouldGroupLogs(Log &log, Log &prevLog);
			bool mergeGroup(GroupIterator group);

			LogIterator updateLogs();
			void updateGroups(LogIterator newBegin, LogIterator newEnd);

			contacts_filter_h getFilter();
			contacts_list_h fetchLogList();

			void onLogsChanged(const char *viewUri);
			void onContactChanged(const char *viewUri);

			int m_DbVersion;

			LogGroupList m_Groups;
			Contacts::Model::ContactDataList m_Logs;

			InsertCallback m_InsertCallback;
		};
	}
}

#endif /* LOGS_MODEL_LOG_PROVIDER_H */
