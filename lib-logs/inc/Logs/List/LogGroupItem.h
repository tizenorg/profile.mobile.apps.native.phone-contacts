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

#ifndef LOGS_LIST_GROUP_ITEM_H
#define LOGS_LIST_GROUP_ITEM_H

#include "Ui/GenlistGroupItem.h"
#include <time.h>

namespace Logs
{
	namespace List
	{
		/**
		 * @brief Group log item object
		 */
		class LogGroupItem : public Ui::GenlistGroupItem
		{
		public:
			/**
			 * @brief Create new log group item object
			 * @param[in]   date   Group log item date
			 */
			LogGroupItem(const tm &date);

			/**
			 * @brief Get group date
			 * @return group date.
			 */
			const tm &getDate() const;

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;

			char *convertDate(const tm &date);

			tm m_Date;
		};
	}
}

#endif /* LOGS_LIST_GROUP_ITEM_H */
