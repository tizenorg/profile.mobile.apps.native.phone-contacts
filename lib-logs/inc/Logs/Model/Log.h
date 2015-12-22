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
			virtual bool isGroup() const = 0;

			/**
			 * @return log record
			 */
			virtual const contacts_record_h getLogRecord() const = 0;

			/**
			 * @return log name
			 */
			virtual const char *getName() const = 0;

			/**
			 * @return log number
			 */
			virtual const char *getNumber() const = 0;

			/**
			 * @return log name image path
			 */
			virtual const char *getImagePath() const = 0 ;

			/**
			 * @return log type
			 */
			virtual int getType() const = 0;

			/**
			 * @return log time
			 */
			virtual struct tm getTime() const = 0;

			/**
			 * @return log id
			 */
			virtual int getId() const = 0;

			/**
			 * @return log person id
			 */
			virtual int getPersonId() const = 0;
		};
	}
}

#endif /* LOGS_MODEL_LOG_H */
