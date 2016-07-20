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

#ifndef WIDGET_ITEM_H
#define WIDGET_ITEM_H

#include <contacts.h>
#include <functional>
#include "Common/Actions.h"

/**
 * @brief Widget item.
 */
class WidgetItem
{
public:
	/**
	 * @brief Called after item's data was changed.
	 * @param[in]   Change information (@ref ChangeType)
	 */
	typedef std::function<void(int)> ChangeCallback;

	/**
	 * @brief Called before item is removed
	 */
	typedef std::function<void()> DeleteCallback;

	/**
	 * @brief Designates what data was changed.
	 */
	enum ChangeType
	{
		ChangeNone = 0,
		ChangeName = 1 << 0,
		ChangeImage = 1 << 1
	};

	/**
	 * @brief Create new widget item.
	 * @param[in]   actionType  Item action type
	 * @param[in]   dataId      Contacts database phone or email ID
	 */
	WidgetItem(Common::ActionType actionType, int dataId);

	/**
	 * @brief Create object for existing item loaded from database.
	 * @param[in]   id          Widget database item ID
	 * @param[in]   actionType  Item action type
	 * @param[in]   dataId      Contacts database phone or email ID
	 */
	WidgetItem(int id, Common::ActionType actionType, int dataId);
	WidgetItem(const WidgetItem &) = delete;
	WidgetItem(WidgetItem &&);
	virtual ~WidgetItem();

	/**
	 * @brief Set item update callback.
	 * @param[in]   callback    Callback to be called after item was updated.
	 */
	void setChangeCallback(ChangeCallback callback);

	/**
	 * @brief Set item delete callback.
	 * @param[in]   callback    Callback to be called before item is deleted.
	 */
	void setDeleteCallback(DeleteCallback callback);

	/**
	 * @brief Update item data by re-fetching it from Contacts database.
	 * @return true on success, otherwise false
	 */
	bool update();

	/**
	 * @return Widget database item ID.
	 */
	int getId() const;

	/**
	 * @brief Set item ID.
	 * @param[in]   Widget database item ID
	 */
	void setId(int id);

	/**
	 * @return Widget item action type.
	 */
	Common::ActionType getActionType() const;

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
	 * @return Associated contact name.
	 */
	const char *getName() const;

	/**
	 * @return Associated contact image path.
	 */
	const char *getImagePath() const;

	WidgetItem & operator=(const WidgetItem &) = delete;
	WidgetItem & operator=(WidgetItem &&);

private:
	contacts_record_h fetchRecord(int dataId);
	int getChanges(contacts_record_h oldRecord, contacts_record_h newRecord);

	int m_Id;
	Common::ActionType m_ActionType;
	const struct Metadata *m_Metadata;
	contacts_record_h m_Record;

	ChangeCallback m_OnChanged;
	DeleteCallback m_OnDelete;
};

#endif /* WIDGET_ITEM_H */
