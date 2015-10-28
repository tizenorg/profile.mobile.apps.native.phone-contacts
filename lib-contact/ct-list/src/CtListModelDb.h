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

#ifndef __CT_LIST_MODEL_DB_H_
#define __CT_LIST_MODEL_DB_H_

#include <map>
#include <vector>
#include <string>
#include <contacts.h>
#include <memory>
#include <Ecore.h>
#include <Elementary.h>
#include <mutex>

#include "ContactsCommon.h"
#include "ICtChangeListener.h"
#include "CtCommon.h"
#include "CtType.h"
#include "CtThread.h"

#include "CtListModelListener.h"
#include "CtListModel.h"
#include "CtListDataPerson.h"

#include "CtListModelFilter.h"


#define CTLISTDATA_NQUERY_COUNT 20

/**
 * @brief Provides access to the database contacts data list.
 */
class CtListModelDb : public CtListModel , public CtListModelListener
{
public:
	/**
	 * @brief Constructor.
	 *
	 * @param[in]   filterType  The type of filter
	 * @param[in]   limit       The limit of checked contacts
	 *
	 * @see CtListHasType
	 */
	CtListModelDb(CtListHasType filterType, int limit);

	/**
	 * @brief Constructor.
	 *
	 * @param[in]   dataType    The type of the contacts data
	 *
	 * @see CtListDataType
	 */
	CtListModelDb(CtListDataType dataType);

	virtual ~CtListModelDb();

public:
	// CtListModel interfaces

	/**
	 * @see CtListModel::getContactsCount()
	 */
	virtual int getContactsCount(CtListQueryType queryType);

	/**
	 * @see CtListModel::getContacts()
	 */
	virtual int getContacts(CtListQueryType queryType, CtGetContactsType syncType);

	/**
	 * @see CtListModel::cancelGetContacts()
	 */
	virtual void cancelGetContacts();

	/**
	 * @see CtListModel::getAtPersonData()
	 */
	virtual std::shared_ptr<CtListDataPerson> getAtPersonData(int index, CtListQueryType queryType);

	/**
	 * @see CtListModel::getPersonListCount()
	 */
	virtual int getPersonListCount(CtListQueryType queryType);

	/**
	 * @see CtListModel::getMadePersonListStatus()
	 */
	virtual bool getMadePersonListStatus();

	/**
	 * @see CtListModel::attachListener()
	 */
	virtual void attachListener(ICtChangeListener *listener);

	/**
	 * @see CtListModel::setSelectedPersonId()
	 */
	virtual void setSelectedPersonId(int selectedPersonId);

	/**
	 * @see CtListModel::getSearchString()
	 */
	virtual const char *getSearchString(void);

	/**
	 * @see CtListModel::setSearchString()
	 */
	virtual bool setSearchString(char *search);

	/**
	 * @see CtListModel::getCheckedDataCount()
	 */
	virtual int getCheckedDataCount();

	/**
	 * @see CtListModel::initialOriginalCheckedIdList()
	 */
	virtual void initialOriginalCheckedIdList();

	/**
	 * @see CtListModel::clearOriginalCheckedIdList()
	 */
	virtual void clearOriginalCheckedIdList();

	/**
	 * @see CtListModel::checkIsSelectAllOnSearching()
	 */
	virtual bool checkIsSelectAllOnSearching();

	/**
	 * @see CtListModel::initialOriginalPersonDataListSize()
	 */
	virtual void initialOriginalPersonDataListSize();

	/**
	 * @see CtListModel::clearOriginalPersonDataListSize()
	 */
	virtual void clearOriginalPersonDataListSize();

	/**
	 * @see CtListModel::getOriginalPersonDataListSize()
	 */
	virtual int getOriginalPersonDataListSize();

	/**
	 * @see CtListModel::getLimitCount()
	 */
	virtual int getLimitCount();


	/**
	 * @brief Pushes data to the data checked list.
	 *
	 * @param[in]   dataPerson  The data to be pushed
	 *
	 * @see CtListDataPerson
	 */
	virtual void pushCheckedData(std::shared_ptr<CtListDataPerson> dataPerson);

	/**
	 * @brief Removes data from the data checked list.
	 *
	 * @param[in]   dataPerson  The data to be removed
	 *
	 * @see CtListDataPerson
	 */
	virtual void removeCheckedData(std::shared_ptr<CtListDataPerson> dataPerson);

	// CtListModelListener

	/**
	 * @see CtListModelListener::onFilterChanged()
	 */
	virtual void onFilterChanged();

	/**
	 * @brief Gets data list of checked contacts.
	 *
	 * @return data list.
	 *
	 * @see CtListDataPerson
	 */
	std::vector<std::shared_ptr<CtListDataPerson>>* getCheckedDataList() const;

	/**
	 * @brief Checks if contacts is MFC(most frequent contact).
	 *
	 * @param[in]   personId    Contact to be checked
	 *
	 * @return true if contact is present in the list.
	 */
	bool checkMostFrequentContact(int personId);

	/**
	 * @brief Changes data's checked state with no relation.
	 *
	 * @param[in]   dataPerson  The data which checked state should be changed
	 * @param[in]   check       True if should be checked
	 *
	 * @see CtListDataPerson
	 */
	void doCheckedDataNoRelation(std::shared_ptr<CtListDataPerson> dataPerson, bool check);



private:

	void __initialize(void);
	int __getQueryResult(int offset, int limit);
	CtListDataPerson* __createPersonData(contacts_record_h record);
	bool __makePersonList(contacts_list_h list, int offset);
	void __getMfcPersonListFromPreference();
	int __setImage(contacts_record_h record, char *image);
	bool __setMFCImage(contacts_record_h personRecord, char *imagePath);
	void __getMfcPersonList();
	void __getFavoritePersonList();
	bool __makeMfcPersonList(contacts_list_h list);
	bool __makeFavoritePersonList(contacts_list_h list);
	void __setAsCheck(std::shared_ptr<CtListDataPerson> personData, bool check, bool noRelation);
	bool __checkToDisplayMfc(char *addressbookIds);

private:
	static void __onDbChanged(const char* view_uri, void* user_data);
	static Eina_Bool __onGetContactsIdlerCb(void* data);
	static void __onGetContactsThreadRunCb(void *data);
	static void __onGetContactsEndCb(void *data);
	static void __onDisplayNameOrderChangedCb(contacts_name_display_order_e name_display_order, void* user_data);
	static void __onNameSortingOrderChangedCb(contacts_name_sorting_order_e name_display_order, void* user_data);
	static Eina_Bool __getCurentMfcListIdlerCb(void* data);
	static void __onGetCurrentMfcListThreadRunCb(void* data);
	static void __onGetCurrentMfcListThreadEndCb(void* data);

private:
	std::string __search;
	CtListDataType __dataType;
	ICtChangeListener *__listener;

	CtThread *__listThread;
	CtThread *__mfcThread;

	Ecore_Idler *__idler;
	Ecore_Idler *__updateMFCListIdler;
	int __offset;
	CtListModelFilter *__dataFilter;
	int __selectedPersonId;
	int __limitCount;
	bool __madePersonList;
	int __excludeAddressbookId;
	bool __needUpdate;
	int __originalCount;
	int __curCheckCount;

	std::vector<std::pair<int, std::shared_ptr<CtListDataPerson>>> __personDataList;
	std::vector<std::pair<int, std::shared_ptr<CtListDataPerson>>> __mfcPersonDataList;
	std::vector<std::pair<int, std::shared_ptr<CtListDataPerson>>> __favoritePersonDataList;

	std::map<int, std::shared_ptr<CtListDataPerson>> __checkedDataList;
	std::map<int, std::shared_ptr<CtListDataPerson>> __checkedDataListOld;

	std::vector<int> __excludeIdList;
	std::vector<int> __originalCheckedIdList; // when searching, use this list to check
	int __originalPersonDataListSize; // when searching, use this size to check

	std::vector<CtAddressbookInfo*>	__addressbookList;

	std::mutex __mutexLock;
};

#endif

