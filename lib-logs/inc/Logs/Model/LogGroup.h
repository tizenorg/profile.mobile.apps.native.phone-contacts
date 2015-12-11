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
#include "Logs/Model/LogType.h"

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
			virtual bool isGroup() const override;
			virtual const char *getName() const override;
			virtual const char *getNumber() const override;
			virtual const char *getImagePath() const override;
			virtual int getType() const override;
			virtual int getTime() const override;
			virtual int getId() const override;

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

