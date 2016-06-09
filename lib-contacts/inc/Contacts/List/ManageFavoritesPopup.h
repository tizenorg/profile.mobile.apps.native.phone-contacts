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

#ifndef CONTACTS_LIST_MANAGE_FAVORITES_POPUP_H
#define CONTACTS_LIST_MANAGE_FAVORITES_POPUP_H

#include "Ui/ListPopup.h"

namespace Ui
{
	class Navigator;
}

namespace Contacts
{
	namespace List
	{
		/**
		 * @brief The popup provides ability to add/reorder favorites and
		 * remove most frequent and favorites contacts
		 */
		class ManageFavoritesPopup : public Ui::ListPopup
		{
		public:
			/**
			 * @brief Whether Mfc contacts were updated
			 */
			typedef std::function<void()> MfcUpdateCallback;

			/**
			 * @brief Creates new popup
			 * @param[in]   navigator   Current navigator. Is needed to move to the next view.
			 */
			ManageFavoritesPopup(Ui::Navigator *navigator);

			/**
			 * @brief Set Mfc update callback
			 * @param[in]   callback    Mfc update callback
			 */
			void setMfcUpdateCallback(MfcUpdateCallback callback);

		protected:
			/**
			 * @see Control::onCreated()
			 */
			virtual void onCreated() override;

		private:
			ManageFavoritesPopup(const ManageFavoritesPopup &that) = delete;
			ManageFavoritesPopup & operator=(const ManageFavoritesPopup &that) = delete;

			void onAdd();
			void onReorder();
			void onRemove();

			Ui::Navigator *m_Navigator;
			MfcUpdateCallback m_OnMfcUpdated;
		};
	}
}

#endif /* CONTACTS_LIST_MANAGE_FAVORITES_POPUP_H */
