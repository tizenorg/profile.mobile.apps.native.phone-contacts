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

#ifndef SORTBYITEM_H_
#define SORTBYITEM_H_

#include "Ui/GenlistItem.h"

namespace Contacts
{
	namespace Settings
	{
		/**
		 * @brief SortBy item
		 */
		class EXPORT_API SortByItem : public Ui::GenlistItem
		{
		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
			static void onSortingOrderChanged(contacts_name_sorting_order_e name_sorting_order, void *user_data);
			virtual void onSelected() override;
		};
	}
}

#endif /* SORTBYITEM_H_ */
