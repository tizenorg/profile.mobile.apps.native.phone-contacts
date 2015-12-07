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
#include <contacts.h>

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
			 * @param[in]   record  Contact record (_contact_person_phone_log)
			 */
			LogRecord(contacts_record_h record);
			virtual ~LogRecord();
			virtual bool isGroup() const override;
			virtual const char *getName() const override;
			virtual const char *getNumber() const override;
			virtual const char *getImagePath() const override;
			virtual int getType() const override;
			virtual int getTime() const override;
			virtual int getId() const override;

		private:
			contacts_record_h m_Record;
		};
	}
}
#endif /* LOGS_MODEL_LOG_RECORD_H */
