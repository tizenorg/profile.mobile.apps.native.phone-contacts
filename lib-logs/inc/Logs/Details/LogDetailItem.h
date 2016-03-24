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

#ifndef LOGS_DETAILS_LOG_DETAIL_ITEM_H
#define LOGS_DETAILS_LOG_DETAIL_ITEM_H

#include "Contacts/SelectItem.h"

namespace Logs
{
	namespace Model
	{
		class Log;
	}

	namespace Details
	{
		class LogDetailItem : public Contacts::SelectItem
		{
		public:

			/**
			 * @brief Create log item
			 * @param[in]   log   Log
			 */
			LogDetailItem(Model::Log *log);

			/**
			 * @return log id
			 */
			int getLogId();

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;
			virtual void onSelectModeChanged(Contacts::SelectMode selectMode) override;
			virtual Contacts::SelectResult getDefaultResult() const override;

			static Evas_Object *createIcon(Evas_Object *parent, const char *path);
			static const char *getImagePath(int type);
			static char *getTypeString(int type, time_t duration);
			static char *appendDuration(const char *typeStr, time_t duration);

			Model::Log *m_Log;
		};
	}
}

#endif /* LOGS_DETAILS_LOG_DETAIL_ITEM_H */
