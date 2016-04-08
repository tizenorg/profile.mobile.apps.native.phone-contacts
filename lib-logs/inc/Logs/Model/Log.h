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

#include "Contacts/Model/ContactData.h"

#include <contacts.h>
#include <time.h>
#include <functional>
#include <string>

namespace Logs
{
	namespace Model
	{
		class LogGroup;
		/**
		 * @brief Log record information
		 */
		class Log : public Contacts::Model::ContactData
		{
		public:
			/**
			 * @brief Callback to be called when log was removed to group.
			 * @param[in]   log   Removed log
			 */
			typedef std::function<void()> RemoveCallback;

			/**
			 * @brief Create log record
			 * @param[in]   record  Log record (_contact_person_phone_log)
			 */
			Log(contacts_record_h record);
			~Log();
			Log(const Log &log) = delete;
			Log &operator=(const Log &log) = delete;

			/**
			 * @brief Set log remove callback.
			 * @param[in]   callback    log remove callback
			 */
			void setRemoveCallback(RemoveCallback callback);

			/**
			 * @brief Unset log remove callback.
			 */
			void unsetRemoveCallback();

			/**
			 * @return log id
			 */
			virtual int getId() const override;

			/**
			 * @return log name
			 */
			virtual const char *getName() const override;

			/**
			 * @return log number
			 */
			virtual const char *getNumber() const override;

			/**
			 * @return log name image path
			 */
			virtual const char *getImagePath() const override;

			/**
			 * @see ContactData::compare()
			 * @detail Compares by number
			 */
			virtual int compare(const char *str, int pattern) override;

			/**
			 * @return type of number
			 */
			int getNumberType() const;

			/**
			 * @return label of number, when the number type is CONTACTS_NUMBER_TYPE_CUSTOM
			 */
			std::string getNumberLabel() const;

			/**
			 * @return log type
			 */
			int getType() const;

			/**
			 * @return log time
			 */
			tm getTime() const;

			/**
			 * @return log person id
			 */
			int getPersonId() const;

			/**
			 * @return log contact id
			 */
			int getContactId() const;

			/**
			 * @return log duration in seconds
			 */
			time_t getDuration() const;

			/**
			 * @brief  Set log group
			 * @param[in]   logGroup  Parent log group
			 */
			void setLogGroup(LogGroup *group);

			/**
			 * @return Parent log group
			 */
			LogGroup *getLogGroup() const;

			/**
			 * @brief Update log record
			 * @param[in]   record  Log record
			 */
			void update(contacts_record_h record);

			/**
			 * @brief Update contact record
			 */
			void updateContactRecord();
			/**
			 * @return Remove log from database
			 */
			void remove();

		private:
			contacts_record_h getContactRecord();

			contacts_record_h m_LogRecord;
			contacts_record_h m_ContactRecord;

			LogGroup *m_Group;
			RemoveCallback m_OnRemoved;
		};
	}
}
#endif /* LOGS_MODEL_LOG_H */
