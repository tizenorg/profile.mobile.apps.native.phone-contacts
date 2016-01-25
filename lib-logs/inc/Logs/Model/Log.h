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

#ifndef LOGS_MODEL_LOG_H
#define LOGS_MODEL_LOG_H

#include "Logs/Model/LogGroup.h"
#include "Logs/Model/LogType.h"

namespace Logs
{
	namespace Model
	{
		/**
		 * @brief Log record information
		 */
		class Log
		{
		public:
			/**
			 * @brief Create log record
			 * @param[in]   record  Contact record (_contact_person_phone_log)
			 */
			Log(contacts_record_h record);

			virtual ~Log();

			Log(const Log &log) = delete;

			Log &operator=(const Log &log) = delete;

			/**
			 * @return log name
			 */
			const char *getName() const;

			/**
			 * @return log number
			 */
			const char *getNumber() const;

			/**
			 * @return log name image path
			 */
			const char *getImagePath() const;

			/**
			 * @return log type
			 */
			int getType() const;

			/**
			 * @return log time
			 */
			struct tm getTime() const;

			/**
			 * @return log id
			 */
			int getId() const;

			/**
			 * @return log person id
			 */
			int getPersonId() const;

			/**
			 * @brief  Update log;
			 */
			void updateLog();

			/**
			 * @brief  Set log group;
			 * @param[in]   logGroup  Parent log group
			 */
			void setLogGroup(LogGroup *group);

			/**
			 * @brief  Get log group;
			 * @return  logGroup  Parent log group
			 */
			LogGroup *getLogGroup();

		private:
			contacts_record_h getContactRecord();

			contacts_record_h m_LogRecord;
			contacts_record_h m_ContactRecord;

			LogGroup *m_Group;
		};
	}
}
#endif /* LOGS_MODEL_LOG_H */
