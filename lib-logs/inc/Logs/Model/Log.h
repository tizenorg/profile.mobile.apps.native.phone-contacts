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

#include <contacts.h>
#include <time.h>
#include "Logs/Model/LogType.h"

namespace Logs
{
	namespace Model
	{
		/**
		 * @brief Log information
		 */
		class Log
		{
		public:
			virtual ~Log() { }
			/**
			 * @return is log group
			 */
			virtual bool isGroup() const { return false; }

			/**
			 * @return log record
			 */
			virtual const contacts_record_h getLogRecord() const { return nullptr; }

			/**
			 * @return log name
			 */
			virtual const char *getName() const { return nullptr; }

			/**
			 * @return log number
			 */
			virtual const char *getNumber() const { return nullptr; }

			/**
			 * @return log name image path
			 */
			virtual const char *getImagePath() const { return nullptr; }

			/**
			 * @return log type
			 */
			virtual int getType() const { return CONTACTS_PLOG_TYPE_NONE; }

			/**
			 * @return log time
			 */
			virtual struct tm getTime() const { return {0}; }

			/**
			 * @return log id
			 */
			virtual int getId() const { return 0; }

			/**
			 * @return log person id
			 */
			virtual int getPersonId() const { return 0; }

			/**
			 * @return is person id changed
			 * @param[in]    personId    Person Id
			 */
			virtual bool isPersonIdChanged(int personId) { return false; }

			/**
			 * @return is log name changed
			 */
			virtual bool isLogNameChanged() { return false; }

			/**
			 * @brief Set log changed callback
			 * @remark Callback called when log is changed.
			 * @param[in]    callback    Changed log callback
			 */
			virtual void setLogChangeCallback(LogChangeCallback callback)
			{
				m_LogChangeCallback = std::move(callback);
			}

			/**
			 * @brief Unset log change callback
			 */
			virtual void unsetLogChangeCallback()
			{
				m_LogChangeCallback = nullptr;
			}

			/**
			 * @brief Get log change callback
			 */
			virtual void callLogChangeCallback()
			{
				if (m_LogChangeCallback) {
					m_LogChangeCallback();
				}
			}

			/**
			 * @brief Set log remove callback
			 * @remark Callback called when log removed.
			 * @param[in]    callback    Removed log callback
			 */
			virtual void setLogRemoveCallback(LogRemoveCallback callback)
			{
				m_LogRemoveCallback = std::move(callback);
			}

			/**
			 * @brief Unset log remove  callback
			 */
			virtual void unsetLogRemoveCallback()
			{
				m_LogRemoveCallback = nullptr;
			}

			/**
			 * @brief Get log remove callback
			 */
			virtual void callLogRemoveCallback()
			{
				if (m_LogRemoveCallback) {
					m_LogRemoveCallback();
				}
			}

		private:
			LogChangeCallback m_LogChangeCallback;
			LogRemoveCallback m_LogRemoveCallback;
		};
	}
}

#endif /* LOGS_MODEL_LOG_H */
