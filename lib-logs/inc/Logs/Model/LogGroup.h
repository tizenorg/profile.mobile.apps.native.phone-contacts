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

namespace Logs
{
	namespace Model
	{
		/**
		 * @brief Group of logs
		 */
		class LogGroup : public Log
		{
		public:

			/**
			 * @see Log::isGroup()
			 */
			virtual bool isGroup() const override;

			/**
			 * @see Log::getLogRecord()
			 */
			virtual const contacts_record_h getLogRecord() const override;

			/**
			 * @see Log::getName()
			 */
			virtual const char *getName() const override;

			/**
			 * @see Log::getNumber()
			 */
			virtual const char *getNumber() const override;

			/**
			 * @see Log::getImagePath()
			 */
			virtual const char *getImagePath() const override;

			/**
			 * @see Log::getType()
			 */
			virtual int getType() const override;

			/**
			 * @see Log::getTime()
			 */
			virtual struct tm getTime() const override;

			/**
			 * @see Log::getId()
			 */
			virtual int getId() const override;

			/**
			 * @see Log::getPersonId()
			 */
			virtual int getPersonId() const override;

			/**
			 * @see Log::setContactChangeCallback()
			 */
			virtual void setContactChangeCallback(ContactChangeCallback callback) override;

			/**
			 * @see Log::unsetContactChangeCallback()
			 */
			virtual void unsetContactChangeCallback() override;

			/**
			 * @see Log::callContactChangeCallback()
			 */
			virtual void callContactChangeCallback(LogPtr log, contacts_changed_e type) override;

			/**
			 * @brief Remove log from LogGroup by id
			 * @param[in]   id  Log id
			 */
			void removeLog(int id);

			/**
			 * @brief Add log to LogGroup
			 * @param[in]   log  Log to add
			 */
			void addLog(LogPtr log);

			/**
			 * @brief Get log group list
			 * @return list of log group. The reference will be valid till this LogGroup object exist.
			 */
			const LogList &getLogList() const;

		private:
			LogList m_LogList;
		};
	}
}
#endif /* LOGS_MODEL_LOG_GROUP_H */

