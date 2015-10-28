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

#ifndef __CT_LIST_MODEL_H_
#define __CT_LIST_MODEL_H_

#include <vector>
#include <memory>
#include "CtListDataPerson.h"
#include "ICtChangeListener.h"

/**
 * @brief Specifies how list should be filled (Immediately or with delay).
 */
typedef enum {
	GET_AS_SYNC,
	GET_AS_ASYNC,
	GET_AS_SYNC_ASYNC,
} CtGetContactsType;

/**
 * @brief Specifies the type of contacts for which query is created.
 */
typedef enum {
	LIST_QUERY_DEFAULT,
	LIST_QUERY_FAVORITE,
	LIST_QUERY_MFC,
} CtListQueryType;

/**
 * @brief Address book information.
 */
typedef struct {
	int addressbookId;
	int accountId;
	char *iconPath;
	char *addressbookName;
	char *displayName;
	bool display;
	int mode;
} CtAddressbookInfo;

/**
 * @brief Provides access to the contacts data list.
 */
class CtListModel
{
public:
	virtual ~CtListModel(){ };

public:
	/**
	 * @brief Gets specified contacts count from database.
	 *
	 * @param[in]   queryType   The type of the contacts which will be counted
	 *
	 * @return contacts count.
	 *
	 * @see CtListQueryType
	 */
	virtual int getContactsCount(CtListQueryType queryType) = 0;

	/**
	 * @brief Gets specified contacts list according to @a syncType.
	 *
	 * @param[in]   queryType   The type which specifies person data list
	 * @param[in]   syncType    The type which specifies how the list will be filled
	 *
	 * @return contacts count.
	 *
	 * @see CtListQueryType
	 * @see CtGetContactsType
	 */
	virtual int getContacts(CtListQueryType queryType, CtGetContactsType syncType) = 0;

	/**
	 * @brief Cancels getting of contacts list.
	 */
	virtual void cancelGetContacts() { };

	/**
	 * @brief Gets person data.
	 *
	 * @param[in]   index       The id of the contact whose person data should be returned
	 * @param[in]   queryType   The type which specifies person data list
	 *
	 * @return person data.
	 *
	 * @see CtListDataPerson
	 * @see CtListQueryType
	 */
	virtual std::shared_ptr<CtListDataPerson> getAtPersonData(int index, CtListQueryType queryType) = 0;

	/**
	 * @brief Gets contacts count from person data list.
	 *
	 * @param[in]   queryType   The type of the contacts which will be counted
	 *
	 * @return contacts count.
	 *
	 * @see CtListQueryType
	 */
	virtual int getPersonListCount(CtListQueryType queryType) = 0;

	/**
	 * @brief Gets status of whether person data list has been updated from database.
	 *
	 * @return true if from database.
	 */
	virtual bool getMadePersonListStatus() { return true; }

	/**
	 * @brief Attaches listener.
	 *
	 * @param[in]   listener    The listener to be attached.
	 *
	 * @see ICtChangeListener
	 */
	virtual void attachListener(ICtChangeListener *listener) = 0;

	/**
	 * @brief Sets selected person id.
	 *
	 * @param[in]   selectedPersonId    The selected person id to be set.
	 */
	virtual void setSelectedPersonId(int selectedPersonId) { };

	/**
	 * @brief Gets search string.
	 *
	 * @return search string.
	 */
	virtual const char *getSearchString(void) { return NULL; };

	/**
	 * @brief Sets search string.
	 *
	 * @param[in]   search  The string to be set as searched.
	 *
	 * @return true if set.
	 */
	virtual bool setSearchString(char *search) { return false; };

	/**
	 * @brief Gets checked data count.
	 *
	 * @return checked data count.
	 */
	virtual int getCheckedDataCount() { return 0; };

	/**
	 * @brief Initializes original checked id list.
	 */
	virtual void initialOriginalCheckedIdList() { };

	/**
	 * @brief Clears original checked id list.
	 */
	virtual void clearOriginalCheckedIdList() { };

	/**
	 * @brief Checks if all contacts are checked.
	 *
	 * @return true if checked all.
	 */
	virtual bool checkIsSelectAllOnSearching() { return false; };

	/**
	 * @brief Initializes original person data list size.
	 */
	virtual void initialOriginalPersonDataListSize() { };

	/**
	 * @brief Clears original person data list size.
	 */
	virtual void clearOriginalPersonDataListSize() { };

	/**
	 * @brief Gets original person data list size.
	 *
	 * @return list size.
	 */
	virtual int getOriginalPersonDataListSize() { return 0; };

	/**
	 * @brief Gets limit count of checked contacts.
	 *
	 * @return limit count.
	 */
	virtual int getLimitCount() { return 0; };
};

#endif
