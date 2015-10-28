/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef _WIDGET_ITEM_H_
#define _WIDGET_ITEM_H_

#include <contacts.h>
#include "WidgetItemType.h"

/**
 * @brief Widget item.
 */
class WidgetItem
{
public:
	/**
	 * @brief Create new widget item.
	 * @param[in]   type    Item type
	 * @param[in]   dataId  Contacts database phone or email ID
	 */
	WidgetItem(WidgetItemType type, int dataId);

	/**
	 * @brief Create object of existing item loaded from database.
	 * @param[in]   id      Widget database item ID
	 * @param[in]   type    Item type
	 * @param[in]   dataId  Contacts database phone or email ID
	 */
	WidgetItem(int id, WidgetItemType type, int dataId);
	WidgetItem(const WidgetItem&) = delete;
	WidgetItem(WidgetItem&&);
	virtual ~WidgetItem();

	/**
	 * @brief Update item data by re-fetching it from contacts database.
	 * @return true on success, otherwise false
	 */
	bool update();

	/**
	 * @return User data associated with the item.
	 * @see setUserData
	 */
	void *getUserData() const;

	/**
	 * @brief Associate user data with widget item.
	 * @param[in]   data    User data to be associated with the item
	 */
	void setUserData(void *data);

	/**
	 * @return Widget database item ID.
	 */
	int  getId() const;

	/**
	 * @brief Set item ID.
	 * @param[in]   Widget database item ID
	 */
	void setId(int id);

	/**
	 * @return Widget item type.
	 */
	WidgetItemType getType() const;

	/**
	 * @remark _contacts_number or _contacts_email depending on item type.
	 * @return Contacts database ID.
	 */
	int getDataId() const;

	/**
	 * @return Contact ID to which item data (number or email) belongs.
	 */
	int getContactId() const;

	/**
	 * @return Item data (number or email).
	 */
	const char *getData() const;

	/**
	 * @return Associated contact display name.
	 */
	const char *getDisplayName() const;

	/**
	 * @return Associated contact thumbnail image path.
	 */
	const char *getThumbnailPath() const;

	WidgetItem & operator=(const WidgetItem&) = delete;
	WidgetItem & operator=(WidgetItem&&);

private:
	contacts_record_h fetchDataRecord(int dataId);

	void *m_UserData;
	int m_Id;
	WidgetItemType m_Type;
	contacts_record_h m_Record;
};

#endif /* _WIDGET_ITEM_H_ */
