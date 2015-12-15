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

#include "Logs/Model/LogType.h"

namespace Logs
{
	namespace Model
	{
		class LogProvider
		{
		public:
			LogProvider();

		private:
			void fillList();
			LogGroupPtr groupLogs(LogPtr log, LogPtr lastLog);
			const LogList &getLogsList();
			void onLogChanged(const char *viewUri);
			void onContactChanged(const char *viewUri);

			LogList m_AllLogs;
			int m_DbVersion;
		};
	}
}

#endif /* LOGS_MODEL_LOG_PROVIDER_H */
