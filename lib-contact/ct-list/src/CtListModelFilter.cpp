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

#include <contacts.h>
#include <vector>
#include <memory>
#include <algorithm>
#include <app_preference.h>

#include "CtCommon.h"
#include "CtListModelFilter.h"
#include "CtListModelFilter.h"

CtListModelFilter::CtListModelFilter(CtListHasType filterType)
{
	WENTER();
	__initialize();
	__initAddressbook();
	if(__getDisplayAddressbookCount() == 0)
		__setDisplayAddressbookAll();

	__hasType = filterType;
	WLEAVE();
}

CtListModelFilter::CtListModelFilter(CtListDataType dataType)
{
	WENTER();
	__initialize();
	__initAddressbook();
	__getOpFromDataType(dataType);
	if(__getDisplayAddressbookCount() == 0)
		__setDisplayAddressbookAsPreference();
	__initHasType();


}

CtListModelFilter::~CtListModelFilter()
{
	WHIT();
	preference_unset_changed_cb(CT_KEY_DISPLAY_WITH_NUMBER);
	preference_unset_changed_cb(CT_KEY_LIST_DISPLAY_ADDRESSBOOK);

	for(auto iter = __addressbookList.begin(); iter != __addressbookList.end(); iter++) {
		free(iter->second->addressbookName);
		delete (*iter).second;
	}
	__addressbookList.clear();
}


CtListHasType CtListModelFilter::__getListHasType() const
{
	return __hasType;
}


CtListFavoriteType CtListModelFilter::__getListFavoriteType() const
{
	if( __listOp == CT_GET_LIST_ALL_PERSON_REMOVE_FAVORITE)
		return LIST_FAVORITE_SET;
	else if (__listOp == CT_GET_LIST_ALL_PERSON_ADD_FAVORITE)
		return LIST_FAVORITE_UNSET;
	else
		return LIST_FAVORITE_NONE;
}
void CtListModelFilter::setListHasType(CtListHasType hasType)
{
	WDEBUG("hasType %d", hasType);
	__hasType = hasType;
}

void CtListModelFilter::attachListener(CtListModelListener *listener)
{
	__listener = listener;
}

contacts_query_h CtListModelFilter::getQuery(CtListQueryType queryType)
{
	WDEBUG("queryType %d", queryType);

	contacts_query_h query = NULL;
	contacts_filter_h filter = NULL;
	contacts_filter_h filter2 = NULL;
	contacts_filter_h filter3 = NULL;
	contacts_filter_h filter4 = NULL;
	int err = CONTACTS_ERROR_NONE;
	int conditionCount = 0;

	unsigned int person_projection[] = {
		_contacts_person.id,
		_contacts_person.display_name,
		_contacts_person.display_name_index,
		_contacts_person.addressbook_ids,
		_contacts_person.image_thumbnail_path,
		_contacts_person.is_favorite,
	};

	unsigned int person_contact_projection[] = {
		_contacts_person_contact.person_id,
		_contacts_person_contact.display_name,
		_contacts_person_contact.display_name_index,
		_contacts_person_contact.addressbook_ids,
		_contacts_person_contact.image_thumbnail_path,
	};

	unsigned int person_usage_projection[] = {
		_contacts_person_usage.person_id,
		_contacts_person_usage.display_name,
		_contacts_person_usage.image_thumbnail_path,
	};

	if(queryType == LIST_QUERY_MFC) {
		WDEBUG("person usage uri");

		err = contacts_query_create(_contacts_person_usage._uri, &query);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_create() Failed(%d)", err);
		err = contacts_filter_create(_contacts_person_usage._uri, &filter);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
		err = contacts_filter_create(_contacts_person_usage._uri, &filter2);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
		do {
			if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_int(filter2, _contacts_person_usage.times_used, CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, 1))) break;
			if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_operator(filter2, CONTACTS_FILTER_OPERATOR_AND))) break;
			if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_int(filter2, _contacts_person_usage.usage_type, CONTACTS_MATCH_EQUAL, CONTACTS_USAGE_STAT_TYPE_OUTGOING_CALL))) break;
			if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_operator(filter2, CONTACTS_FILTER_OPERATOR_AND))) break;
			if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_bool(filter2, _contacts_person_usage.is_favorite, false))) break;
		} while (0);
		err = contacts_filter_add_filter(filter, filter2);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
		conditionCount++;
		// add condition for hastype
		WDEBUG("condition hastype %d", __getListHasType());
		if(__getListHasType() == LIST_HAS_NUMBER) {
			if(conditionCount > 0) {
				err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
			}

			err = contacts_filter_create(_contacts_person_usage._uri, &filter3);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
			err = contacts_filter_add_bool(filter3, _contacts_person_usage.has_phonenumber, true);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
			err = contacts_filter_add_filter(filter, filter3);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
			conditionCount++;
		}
		else if(__getListHasType() == LIST_HAS_EMAIL) {
			if(conditionCount > 0) {
				err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
			}

			err = contacts_filter_create(_contacts_person_usage._uri, &filter3);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
			err = contacts_filter_add_bool(filter3, _contacts_person_usage.has_email, true);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
			err = contacts_filter_add_filter(filter, filter3);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
			conditionCount++;
		}
		else if(__getListHasType() == LIST_HAS_NUMBER_OR_EMAIL) {
			if(conditionCount > 0) {
				err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
			}

			err = contacts_filter_create(_contacts_person_usage._uri, &filter3);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
			err = contacts_filter_add_bool(filter3, _contacts_person_usage.has_phonenumber, true);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
			err = contacts_filter_add_operator(filter3, CONTACTS_FILTER_OPERATOR_OR);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
			err = contacts_filter_add_bool(filter3, _contacts_person_usage.has_email, true);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
			err = contacts_filter_add_filter(filter, filter3);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
			conditionCount++;
		}

		err = contacts_query_set_sort(query, _contacts_person_usage.times_used, false);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_set_sort() Failed(%d)", err);

		err = contacts_query_set_projection(query, person_usage_projection, sizeof(person_usage_projection)/sizeof(int));
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_set_projection() Failed(%d)", err);

		err = contacts_query_set_distinct(query, true);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_set_distinct() Failed(%d)", err);
	}
	else {
		int allAddressbookCount = __addressbookList.size();
		int displayAddressbookCount = __getDisplayAddressbookCount();
		WDEBUG("allAddressbookCount %d, displayAddressbookCount %d", allAddressbookCount, displayAddressbookCount);
		// create query and filter

		// create query and filter
		if( allAddressbookCount == displayAddressbookCount) {
			WDEBUG("person uri");
			err = contacts_query_create(_contacts_person._uri, &query);
			WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_query_create() Failed(%d)", err);
			err = contacts_filter_create(_contacts_person._uri, &filter);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
			err = contacts_filter_create(_contacts_person._uri, &filter2);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);

			// add condition for favorite
			CtListFavoriteType favoriteType = LIST_FAVORITE_NONE;
			if(queryType == LIST_QUERY_FAVORITE)
				favoriteType = LIST_FAVORITE_SET;
			else
				favoriteType = __getListFavoriteType();
			WDEBUG("condition favoriteType %d", favoriteType);
			if(favoriteType == LIST_FAVORITE_SET) {
				err = contacts_filter_add_bool(filter2, _contacts_person.is_favorite, true);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
				err = contacts_filter_add_filter(filter, filter2);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
				err = contacts_query_set_sort(query, _contacts_person.favorite_priority, true);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_set_sort() Failed(%d)", err);
				conditionCount++;
			}
			else if(__getListFavoriteType() == LIST_FAVORITE_UNSET) {
				err = contacts_filter_add_bool(filter2, _contacts_person.is_favorite, false);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
				err = contacts_filter_add_filter(filter, filter2);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
				conditionCount++;
			}

			// add condition for hastype
			WDEBUG("condition hastype %d", __getListHasType());
			if(__getListHasType() == LIST_HAS_NUMBER) {
				if(conditionCount > 0) {
					err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
					WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
				}

				err = contacts_filter_create(_contacts_person._uri, &filter3);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
				err = contacts_filter_add_bool(filter3, _contacts_person.has_phonenumber, true);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
				err = contacts_filter_add_filter(filter, filter3);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
				conditionCount++;
			}
			else if(__getListHasType() == LIST_HAS_EMAIL) {
				if(conditionCount > 0) {
					err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
					WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
				}

				err = contacts_filter_create(_contacts_person._uri, &filter3);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
				err = contacts_filter_add_bool(filter3, _contacts_person.has_email, true);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
				err = contacts_filter_add_filter(filter, filter3);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
				conditionCount++;
			}
			else if(__getListHasType() == LIST_HAS_NUMBER_OR_EMAIL) {
				if(conditionCount > 0) {
					err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
					WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
				}

				err = contacts_filter_create(_contacts_person._uri, &filter3);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
				err = contacts_filter_add_bool(filter3, _contacts_person.has_phonenumber, true);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
				err = contacts_filter_add_operator(filter3, CONTACTS_FILTER_OPERATOR_OR);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
				err = contacts_filter_add_bool(filter3, _contacts_person.has_email, true);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
				err = contacts_filter_add_filter(filter, filter3);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
				conditionCount++;
			}
			err = contacts_query_set_projection(query, person_projection, sizeof(person_projection)/sizeof(int));
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_set_projection() Failed(%d)", err);

			err = contacts_query_set_distinct(query, true);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_set_distinct() Failed(%d)", err);

		}
		else {
			WDEBUG("person contact uri");
			err = contacts_query_create(_contacts_person_contact._uri, &query);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_create() Failed(%d)", err);
			err = contacts_filter_create(_contacts_person_contact._uri, &filter);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
			err = contacts_filter_create(_contacts_person_contact._uri, &filter2);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);

			// add condition for addressbook
			for (auto iter = __addressbookList.begin(); iter != __addressbookList.end(); iter++)
			{
				if((*iter).second->display == true) {
					if(conditionCount > 0) {
						err = contacts_filter_add_operator(filter2, CONTACTS_FILTER_OPERATOR_OR);
						WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
					}

					WDEBUG("condition addressbookId %d", (*iter).first);
					err = contacts_filter_add_int(filter2, _contacts_person_contact.address_book_id, CONTACTS_MATCH_EQUAL, (*iter).first);
					WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
					conditionCount++;
				}
			}
			err = contacts_filter_add_filter(filter, filter2);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);

			// add condition for favorite
			CtListFavoriteType favoriteType = LIST_FAVORITE_NONE;
			if(queryType == LIST_QUERY_FAVORITE)
				favoriteType = LIST_FAVORITE_SET;
			else
				favoriteType = __getListFavoriteType();
			WDEBUG("condition favoriteType %d", favoriteType);
			if(favoriteType == LIST_FAVORITE_SET) {
				if(conditionCount > 0) {
					err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
					WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
				}

				err = contacts_filter_create(_contacts_person_contact._uri, &filter3);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
				err = contacts_filter_add_bool(filter3, _contacts_person_contact.is_favorite, true);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
				err = contacts_filter_add_filter(filter, filter3);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
				conditionCount++;
			}
			else if(__getListFavoriteType() == LIST_FAVORITE_UNSET) {
				if(conditionCount > 0) {
					err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
					WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
				}

				err = contacts_filter_create(_contacts_person_contact._uri, &filter3);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
				err = contacts_filter_add_bool(filter3, _contacts_person_contact.is_favorite, false);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
				err = contacts_filter_add_filter(filter, filter3);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
				conditionCount++;
			}

			// add condition for hastype
			WDEBUG("condition hastype %d", __getListHasType());

			if(__getListHasType() == LIST_HAS_NUMBER) {
				if(conditionCount > 0) {
					err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
					WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
				}

				err = contacts_filter_create(_contacts_person_contact._uri, &filter4);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
				err = contacts_filter_add_bool(filter4, _contacts_person_contact.has_phonenumber, true);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
				err = contacts_filter_add_filter(filter, filter4);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
				conditionCount++;
			}
			else if(__getListHasType() == LIST_HAS_EMAIL) {
				if(conditionCount > 0) {
					err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
					WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
				}

				err = contacts_filter_create(_contacts_person_contact._uri, &filter4);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
				err = contacts_filter_add_bool(filter4, _contacts_person_contact.has_email, true);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
				err = contacts_filter_add_filter(filter, filter4);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
				conditionCount++;
			}
			else if(__getListHasType() == LIST_HAS_NUMBER_OR_EMAIL) {
				if(conditionCount > 0) {
					err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
					WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
				}
				conditionCount++;

				err = contacts_filter_create(_contacts_person_contact._uri, &filter4);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
				err = contacts_filter_add_bool(filter4, _contacts_person_contact.has_phonenumber, true);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
				err = contacts_filter_add_operator(filter4, CONTACTS_FILTER_OPERATOR_OR);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator() Failed(%d)", err);
				err = contacts_filter_add_bool(filter4, _contacts_person_contact.has_email, true);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int() Failed(%d)", err);
				err = contacts_filter_add_filter(filter, filter4);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter() Failed(%d)", err);
			}

			err = contacts_query_set_projection(query, person_contact_projection, sizeof(person_contact_projection)/sizeof(int));
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_set_projection() Failed(%d)", err);

			err = contacts_query_set_distinct(query, true);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_set_distinct() Failed(%d)", err);
		}
	}
	WDEBUG("conditionCount %d", conditionCount);

	// set filter to query
	if(conditionCount > 0) {
		err = contacts_query_set_filter(query, filter);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_set_filter() Failed(%d)", err);
	}
	else {
		if(query) {
			err = contacts_query_destroy(query);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_destroy() Failed(%d)", err);
			query = NULL;
		}
	}

	if(filter4) {
		err = contacts_filter_destroy(filter4);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_destroy() Failed(%d)", err);
	}
	if(filter3) {
		err = contacts_filter_destroy(filter3);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_destroy() Failed(%d)", err);
	}
	if(filter2) {
		err = contacts_filter_destroy(filter2);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_destroy() Failed(%d)", err);
	}
	if(filter) {
		err = contacts_filter_destroy(filter);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_destroy() Failed(%d)", err);
	}

	return query;
}

void CtListModelFilter::__initialize()
{
	__listOp = CT_GET_LIST_ALL_PERSON;
	__selectedPersonId = 0;
	__viewId = 0;
	__ugRequest = 0;
	__ugData = NULL;
	__listener = NULL;

	preference_set_changed_cb(CT_KEY_DISPLAY_WITH_NUMBER, __onDisplayWithNumberChangedCb, this);
	preference_set_changed_cb(CT_KEY_LIST_DISPLAY_ADDRESSBOOK, __onDisplayAddressbookChangedCb, this);
}

void CtListModelFilter::__initHasType()
{
	if (__listOp == CT_GET_LIST_ALL_PERSON_HAD_NUMBER)
		__hasType = LIST_HAS_NUMBER;
	else
	{
		int showMode = 0;
		preference_get_int(CT_KEY_DISPLAY_WITH_NUMBER, &showMode);
		WDEBUG("showMode %d", showMode);

		if(showMode == 1)
			__hasType = LIST_HAS_NUMBER;
		else
			__hasType = LIST_HAS_NONE;
	}

	WDEBUG("hasType %d", __hasType);
}

void CtListModelFilter::__onDisplayWithNumberChangedCb(const char *key, void *data)
{
	CtListModelFilter* modelFilter = (CtListModelFilter*)data;

	int showMode = 0;
	preference_get_int(CT_KEY_DISPLAY_WITH_NUMBER, &showMode);
	WDEBUG("showMode %d", showMode);

	if(showMode == 1)
		modelFilter->__hasType = LIST_HAS_NUMBER;
	else
		modelFilter->__hasType = LIST_HAS_NONE;

	if (modelFilter->__listener)
		modelFilter->__listener->onFilterChanged();
}

void CtListModelFilter::__onDisplayAddressbookChangedCb(const char *key, void *data)
{
	WHIT();
	CtListModelFilter* modelFilter = (CtListModelFilter*)data;

	if(modelFilter->__ugRequest == 0) {
		modelFilter->__initAddressbook();
		modelFilter->__setDisplayAddressbookAsPreference();

		if (modelFilter->__listener)
			modelFilter->__listener->onFilterChanged();
	}

}


bool CtListModelFilter::__checkAddressbook() const
{
	int all = __addressbookIds.size();
	int show = 0;

	for(auto iter = __addressbookIds.begin(); iter != __addressbookIds.end(); iter++)
	{
		if((*iter).second == true)
			show++;
	}

	if(all == show)
		return true;
	else
		return false;
}


void CtListModelFilter::__initAddressbook()
{
	WENTER();
	int err = CONTACTS_ERROR_NONE;
	contacts_list_h list = NULL;
	contacts_record_h addressbookRecord = NULL;
	CtAddressbookInfo *addressbookInfo = NULL;

	__addressbookList.clear();

	err = contacts_db_get_all_records(_contacts_address_book._uri, 0, 0, &list);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_all_records() Failed(%d)", err);
	while (CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(list, &addressbookRecord)) {
		addressbookInfo = new CtAddressbookInfo;

		err = contacts_record_get_int(addressbookRecord, _contacts_address_book.id, &addressbookInfo->addressbookId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int(_contacts_address_book.id) Failed(%d)", err);

		err = contacts_record_get_int(addressbookRecord, _contacts_address_book.account_id, &addressbookInfo->accountId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int(_contacts_address_book.account_id) Failed(%d)", err);

		err = contacts_record_get_int(addressbookRecord, _contacts_address_book.mode, &addressbookInfo->mode);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int(_contacts_address_book.account_id) Failed(%d)", err);

		err = contacts_record_get_str(addressbookRecord, _contacts_address_book.name, &addressbookInfo->addressbookName);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str(_contacts_address_book.name) Failed(%d)", err);

		addressbookInfo->iconPath = NULL;
		addressbookInfo->displayName = NULL;
		addressbookInfo->display = false;
		WDEBUG("addressbookId %d accountId %d", addressbookInfo->addressbookId, addressbookInfo->accountId);

		__addressbookList.insert(std::make_pair(addressbookInfo->addressbookId, addressbookInfo));

		contacts_list_next(list);
	}
	err = contacts_list_destroy(list, true);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_destroy() Failed(%d)", err);

	WLEAVE();
}

int CtListModelFilter::__getDisplayAddressbookCount()
{
	int count = 0;

	for(auto iter = __addressbookList.begin(); iter != __addressbookList.end(); iter++) {
		if((*iter).second->display == true)
			count++;
	}

	return count;
}

void CtListModelFilter::__setDisplayAddressbookAsPreference()
{
	int displayCount = 0;
	if(__listOp == CT_GET_LIST_ALL_PERSON_REMOVE_FAVORITE || __listOp == CT_GET_LIST_ALL_PERSON_ADD_FAVORITE) {
		WDEBUG("favorite list is not effected by addressbook setting");
		for(auto iter = __addressbookList.begin(); iter != __addressbookList.end(); iter++) {
			(*iter).second->display = true;
			displayCount++;
			continue;
		}
	}
	else {
		char *string = NULL;
		preference_get_string(CT_KEY_LIST_DISPLAY_ADDRESSBOOK, &string);
		WDEBUG("display addressbookid %s", string);
		int hasCondition = SAFE_STRLEN(string);
		std::unique_ptr<std::vector<int>> addressbookIds(CtListModelFilter::__makeTokenizeAddressbookList(string, ";"));
		free(string);
		for(auto iter = __addressbookList.begin(); iter != __addressbookList.end(); iter++) {
			if (hasCondition == 0) { // display all
				(*iter).second->display = true;
				displayCount++;
				continue;
			}

			if(std::find(addressbookIds->begin(), addressbookIds->end(), (*iter).first) != addressbookIds->end()) {
				(*iter).second->display = true;
				displayCount++;
			}
			else {
				(*iter).second->display = false;
			}

			WDEBUG("addressbookid %d, display %d", (*iter).first, (*iter).second->display);
		}
	}

	if(displayCount == 0) {
		WDEBUG("show all");
		for(auto iter = __addressbookList.begin(); iter != __addressbookList.end(); iter++) {
			(*iter).second->display = true;
			continue;
		}
	}
}

void CtListModelFilter::__setDisplayAddressbookAsAppcontrol(int addressbookId)
{
	WDEBUG("%d", addressbookId);

	for(auto iter = __addressbookList.begin(); iter != __addressbookList.end(); iter++) {
		if((*iter).first == addressbookId) {
			(*iter).second->display = true;
		}
		else {
			(*iter).second->display = false;
		}
	}
}

int addressbookId;
int accountId;

void CtListModelFilter::__setDisplayAddressbookWithoutSim()
{
	WHIT();
	int displayCount = __getDisplayAddressbookCount();

	for(auto iter = __addressbookList.begin(); iter != __addressbookList.end(); iter++) {
		if ((*iter).second->accountId > 0 || (*iter).second->addressbookId == 0) {
			if(displayCount == 0)
				(*iter).second->display = true;
		}
		else {
			(*iter).second->display = false;
		}
	}
}

void CtListModelFilter::__setDisplayAddressbookWithoutSimAndFacebook()
{
	WHIT();
	int displayCount = __getDisplayAddressbookCount();

	for(auto iter = __addressbookList.begin(); iter != __addressbookList.end(); iter++) {
		if ((*iter).second->mode == 101) {
			(*iter).second->display = false;
		}
		else {
			if ((*iter).second->accountId > 0 || (*iter).second->addressbookId == 0) {
				if(displayCount == 0)
					(*iter).second->display = true;
			}
			else {
				(*iter).second->display = false;
			}
		}
	}
}

void CtListModelFilter::__setDisplayAddressbookWithoutFacebook()
{
	WHIT();
	int displayCount = __getDisplayAddressbookCount();

	for(auto iter = __addressbookList.begin(); iter != __addressbookList.end(); iter++) {
		if ((*iter).second->mode != 101) {
			if(displayCount == 0)
				(*iter).second->display = true;
		}
		else {
			(*iter).second->display = false;
		}
	}
}

void CtListModelFilter::__setDisplayAddressbookAll()
{
	WHIT();
	for(auto iter = __addressbookList.begin(); iter != __addressbookList.end(); iter++) {
		(*iter).second->display = true;
	}
}

std::vector<int>* CtListModelFilter::__makeTokenizeAddressbookList(const char *originString, const char *delimiter)
{
    std::vector<int>* addressbookList(new std::vector<int>);
	char *token = NULL;
	char *copyString;
	char *save;

	if(!originString) {
		WDEBUG("Input string is NULL");
		delete addressbookList;
		return NULL;
	}

	copyString = SAFE_STRDUP(originString);

	if(copyString) {
		token = strtok_r(copyString, delimiter, &save);

		if(token)
			WDEBUG("list item = %s", token);

		while(token) {
			//char *c = SAFE_STRDUP(p);
			int id = atoi(token);

			addressbookList->push_back(id);

			token = strtok_r(NULL, delimiter, &save);

			if(token)
				WDEBUG("list item = %s", token);
		}
		free(copyString);
	}
	return addressbookList;
}

std::map<int, CtAddressbookInfo*>& CtListModelFilter::getAddressbookList()
{
	return __addressbookList;
}

bool CtListModelFilter::__getOpFromDataType(CtListDataType dataType)
{
	switch(dataType){
	case LIST_DATA_DEFAULT :
	case LIST_DATA_ALL_PERSON :
		__listOp = CT_GET_LIST_ALL_PERSON;
		break;

	case LIST_DATA_PERSON_EXCEPT_SIM:
		__listOp = CT_GET_LIST_ALL_PERSON_ADD_FAVORITE;
		break;
	case LIST_DATA_ALL_FAVORITES:
		__listOp = CT_GET_LIST_ALL_PERSON_REMOVE_FAVORITE;
		break;
	case LIST_DATA_PERSON_WITH_NUMBER:
		__listOp = CT_GET_LIST_ALL_PERSON_HAD_NUMBER;
		break;
	case LIST_DATA_PERSON_EXCEPT_FACEBOOK:
		__listOp = CT_GET_LIST_ALL_PERSON;
		__setDisplayAddressbookWithoutFacebook();
		break;


	default:
		WERROR("Invalid dataType(%d)", dataType);
		return false;
	}

	return true;
}
