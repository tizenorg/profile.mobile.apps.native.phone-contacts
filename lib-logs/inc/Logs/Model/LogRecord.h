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

#ifndef LOGS_MODEL_LOG_RECORD_H
#define LOGS_MODEL_LOG_RECORD_H

#include "Logs/Model/Log.h"

namespace Logs
{
	namespace Model
	{
		/**
		 * @brief Log record information
		 */
		class LogRecord : public Log
		{
		public:
			/**
			 * @brief Create log record
			 * @param[in]   record  Contact record (_contact_pers n_phone_log)
			 */
			LogRecord(contacts_record_h record);

			virtual ~LogRecord();

			LogRecord(const LogRecord &log) = delete;

			LogRecord &operator=(const LogRecord &log) = delete;

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
			 * @see Log::setLogChangeCallback()
			 */
			virtual void setContactChangeCallback(ContactChangeCallback callback) override;

			/**
			 * @see Log::unsetLogChangeCallback()
			 */
			virtual void unsetContactChangeCallback() override;

			/**
			 * @see Log::callLogChangedCallback()
			 */
			virtual void callContactChangeCallback(LogPtr log, contacts_changed_e type) override;

		private:
			contacts_record_h getContactRecord();
			contacts_record_h m_LogRecord;
			contacts_record_h m_ContactRecord;

			ContactChangeCallback m_ContactChangeCallback;
		};
	}
}
#endif /* LOGS_MODEL_LOG_RECORD_H */
