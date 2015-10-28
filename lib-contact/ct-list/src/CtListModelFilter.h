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

#ifndef __CT_LIST_DATA_FILTER_H_
#define __CT_LIST_DATA_FILTER_H_

#include <vector>
#include <map>

#include "ContactsCommon.h"
#include "ContactsDebug.h"

#include "CtType.h"
#include "CtListModel.h"
#include "CtListModelListener.h"

/**
 * @brief List operation
 */
typedef enum {
	CT_GET_LIST_ALL_PERSON,
	CT_GET_LIST_ALL_PERSON_HAD_NUMBER,
	CT_GET_LIST_ALL_PERSON_ADD_FAVORITE,
	CT_GET_LIST_ALL_PERSON_REMOVE_FAVORITE,
}CtListOperation;

/**
 * @brief Filter type
 */
typedef enum {
	LIST_HAS_NONE,
	LIST_HAS_NUMBER,
	LIST_HAS_EMAIL,
	LIST_HAS_NUMBER_OR_EMAIL,
}CtListHasType;

/**
 * @brief Represents favorite state
 */
typedef enum {
	LIST_FAVORITE_NONE,
	LIST_FAVORITE_SET,
	LIST_FAVORITE_UNSET,
}CtListFavoriteType;

/**
 * @brief Provides access to filtering contacts data.
 */
class CtListModelFilter
{
public:
	/**
	 * @brief Constructor.
	 *
	 * @param[in]   filterType  The type of filter
	 *
	 * @see CtListHasType
	 */
	CtListModelFilter(CtListHasType filterType);

	/**
	 * @brief Constructor.
	 *
	 * @param[in]   dataType    The type of the contacts data
	 *
	 * @see CtListDataType
	 */
	CtListModelFilter(CtListDataType dataType);

	virtual ~CtListModelFilter();

public:
	/**
	 * @brief Gets query by specified type.
	 *
	 * @param[in]   queryType   The type of the query
	 *
	 * @return query.
	 *
	 * @see CtListQueryType
	 */
	contacts_query_h getQuery(CtListQueryType queryType);

	/**
	 * @brief Gets list of addressbook info.
	 *
	 * @return list.
	 *
	 * @see CtAddressbookInfo
	 */
	std::map<int, CtAddressbookInfo*>& getAddressbookList();

	/**
	 * @brief Sets filter type.
	 *
	 * @param[in]   hasType The type of filter
	 *
	 * @see CtListHasType
	 */
	void setListHasType(CtListHasType hasType);

	/**
	 * @brief Attaches listener.
	 *
	 * @param[in]   listener    The listener to be attached.
	 *
	 * @see CtListModelListener
	 */
	void attachListener(CtListModelListener *listener);

private:
	bool __getOpFromDataType(CtListDataType dataType);
	void __initialize(void);
	void __initHasType();
	bool __checkAddressbook() const;

	CtListHasType __getListHasType() const;
	CtListFavoriteType __getListFavoriteType() const;

	void __initAddressbook();
	int __getDisplayAddressbookCount();
	void __setDisplayAddressbookAsPreference();
	void __setDisplayAddressbookAsAppcontrol(int addressbookId);
	void __setDisplayAddressbookWithoutSim();
	void __setDisplayAddressbookWithoutSimAndFacebook();
	void __setDisplayAddressbookWithoutFacebook();
	void __setDisplayAddressbookAll();
	static std::vector<int>* __makeTokenizeAddressbookList(const char *originString, const char *delimiter);

	static void __onDisplayWithNumberChangedCb(const char *key, void *data);
	static void __onDisplayAddressbookChangedCb(const char *key, void *data);

private:
	CtListOperation __listOp;
	int __selectedPersonId;
	int __viewId;
	int __ugRequest;
	char* __ugData;
	std::vector<int> excludePersonId;
	std::map<int, bool> __addressbookIds;

	CtListHasType __hasType;
	CtListModelListener *__listener;
	std::map<int, CtAddressbookInfo*> __addressbookList;
};

#endif

