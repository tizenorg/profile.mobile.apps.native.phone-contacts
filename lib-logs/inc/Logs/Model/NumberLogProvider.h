/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef LOGS_MODEL_NUMBER_LOG_PROVIDER_H
#define LOGS_MODEL_NUMBER_LOG_PROVIDER_H

#include "Logs/Model/LogProvider.h"

namespace Logs
{
	namespace Model
	{
		/**
		 * @brief Provides list of logs by number
		 */
		class NumberLogProvider : public LogProvider
		{
		public:
			/**
			 * @brief Create Number log provider.
			 * @param[in]   number   Log number
			 */
			explicit NumberLogProvider(std::string number);

		private:
			virtual contacts_filter_h getFilter() override;
			virtual bool shouldGroupLogs(Log &log, Log &prevLog) override;

			std::string m_Number;
		};
	}
}

#endif /* LOGS_MODEL_NUMBER_LOG_PROVIDER_H */
