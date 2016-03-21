/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef CONTACTS_LIST_MANAGE_FAVORITES_CONTROLLER_H
#define CONTACTS_LIST_MANAGE_FAVORITES_CONTROLLER_H

#include <Elementary.h>
#include <tizen.h>

namespace Ui
{
	class Navigator;
	class ListPopup;
}

namespace Contacts
{
	namespace List
	{
		/**
		 * @brief The controller provides ability to add/reorder favorites and
		 * remove most frequent and favorites contacts
		 */
		class EXPORT_API ManageFavoritesController
		{
		public:
			/**
			 * @brief Creates new controller
			 * @param[in]   navigator   Navigator
			 */
			ManageFavoritesController(Ui::Navigator *navigator);
			~ManageFavoritesController();

			/**
			 * @brief Shows manage favorites popup
			 */
			void showPopup();

		private:
			ManageFavoritesController(const ManageFavoritesController &that) = delete;
			ManageFavoritesController & operator=(const ManageFavoritesController &that) = delete;

			void onAdd();
			void onReorder();
			void onRemove();

			Ui::Navigator *m_Navigator;
			Ui::ListPopup *m_Popup;
		};
	}
}

#endif /* CONTACTS_LIST_MANAGE_FAVORITES_CONTROLLER_H */
