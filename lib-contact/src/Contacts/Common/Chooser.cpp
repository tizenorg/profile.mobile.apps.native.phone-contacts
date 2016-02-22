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

#include "Contacts/Common/Chooser.h"
#include "Contacts/Details/DetailsView.h"
#include "Contacts/List/ListView.h"
#include "Utils/Logger.h"

using namespace Contacts;
using namespace Contacts::Common;
using namespace Contacts::List;
using namespace Contacts::Details;
using namespace std::placeholders;

Chooser::Chooser(SelectMode selectMode, ResultType resultType, size_t selectLimit)
	: m_SelectMode(selectMode), m_ResultType(resultType),
	  m_FilterType(getFilterType(m_ResultType)), m_SelectLimit(selectLimit)
{
}

void Chooser::setSelectCallback(SelectCallback callback)
{
	m_OnSelected = std::move(callback);
}

void Chooser::onCreated()
{
	ListView *view = new ListView(m_FilterType);
	view->setSelectMode(m_SelectMode);
	view->setSelectLimit(m_SelectLimit);

	switch (m_ResultType) {
		case ResultPerson:
			view->setSelectCallback(std::bind(&Chooser::onSelected, this, _1));
			break;
		case ResultNumber:
		case ResultEmail:
			if (m_SelectMode == SelectSingle) {
				view->setSelectCallback(std::bind(&Chooser::onSinglePersonSelected, this, _1));
			} else {
				view->setSelectCallback(std::bind(&Chooser::onMultiPersonSelected, this, _1));
			}
			break;
		default:
			break;
	}

	navigateTo(view);
}

void Chooser::onPageAttached()
{
	/* FIXME: Disable naviframe item title by default */
	getPage()->setTitle(nullptr);
}

bool Chooser::onSinglePersonSelected(SelectResults personResults)
{
	const SelectResult &person = *personResults.begin();
	int resultId = getSingleResultId(person.itemId, m_ResultType);

	if (resultId > 0) {
		SelectResult result = { m_ResultType, resultId };
		return onSelected({ &result, 1 });
	}

	/* FIXME: Implement person support in DetailsView */
	DetailsView *view = new DetailsView(getContactId(person.itemId),
			DetailsView::Type(person.type), m_FilterType);
	view->setSelectMode(m_SelectMode);
	view->setSelectCallback(std::bind(&Chooser::onSelected, this, _1));
	navigateTo(view);

	return false;
}

bool Chooser::onMultiPersonSelected(SelectResults personResults)
{
	contacts_person_property_e property = (m_ResultType == ResultNumber)
			? CONTACTS_PERSON_PROPERTY_NUMBER
			: CONTACTS_PERSON_PROPERTY_EMAIL;

	int id = 0;
	std::vector<SelectResult> results;

	for (auto &&result : personResults) {
		contacts_person_get_default_property(property, result.itemId, &id);
		results.push_back({ m_ResultType, id });
	}

	return onSelected({ results.data(), results.size() });
}

bool Chooser::onSelected(SelectResults results)
{
	if (!m_OnSelected || m_OnSelected(results)) {
		delete this;
	}

	return false;
}

int Chooser::getContactId(int personId)
{
	contacts_record_h record = 0;
	int err = contacts_db_get_record(_contacts_person._uri, personId, &record);
	RETVM_IF_ERR(err, 0, "contacts_db_get_record() failed.");

	int id = 0;
	contacts_record_get_int(record, _contacts_person.display_contact_id, &id);
	contacts_record_destroy(record, true);

	return id;
}

int Chooser::getSingleResultId(int personId, ResultType resultType)
{
	int resultId = 0;

	const char *uri = nullptr;
	unsigned personIdProp = 0;
	unsigned resultIdProp = 0;

	switch (resultType) {
		case ResultNumber:
			uri = _contacts_person_number._uri;
			personIdProp = _contacts_person_number.person_id;
			resultIdProp = _contacts_person_number.number_id;
			break;
		case ResultEmail:
			uri = _contacts_person_email._uri;
			personIdProp = _contacts_person_email.person_id;
			resultIdProp = _contacts_person_email.email_id;
			break;
		default:
			return resultId;
	}

	contacts_filter_h filter = nullptr;
	contacts_filter_create(uri, &filter);
	contacts_filter_add_int(filter, personIdProp, CONTACTS_MATCH_EQUAL, personId);

	contacts_query_h query = nullptr;
	contacts_query_create(uri, &query);
	contacts_query_set_projection(query, &resultIdProp, 1);
	contacts_query_set_filter(query, filter);

	int count = 0;
	int err = contacts_db_get_count_with_query(query, &count);
	WARN_IF_ERR(err, "contacts_db_get_count_with_query() failed.");
	if (count == 1) {
		contacts_list_h list = nullptr;
		err = contacts_db_get_records_with_query(query, 0, 1, &list);
		WARN_IF_ERR(err, "contacts_db_get_records_with_query() failed.");

		contacts_record_h record = nullptr;
		contacts_list_get_current_record_p(list, &record);

		contacts_record_get_int(record, resultIdProp, &resultId);
		contacts_list_destroy(list, true);
	}

	contacts_query_destroy(query);
	contacts_filter_destroy(filter);

	return resultId;
}

int Chooser::getFilterType(ResultType resultType)
{
	switch (resultType) {
		case ResultNumber:
			return FilterNumber;
		case ResultEmail:
			return FilterEmail;
		default:
			return FilterNone;
	}
}
