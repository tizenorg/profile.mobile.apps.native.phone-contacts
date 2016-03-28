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
#include "Contacts/SelectItem.h"
#include "Contacts/Details/DetailsView.h"
#include "Contacts/List/ListView.h"
#include "Contacts/Settings/ExportController.h"
#include "Contacts/Utils.h"
#include "Common/Actions.h"

#include "Ui/ListPopup.h"
#include "Utils/Logger.h"

using namespace Common;
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
	m_ListView = new ListView(m_FilterType);
	m_ListView->setSelectMode(m_SelectMode);
	m_ListView->setSelectLimit(m_SelectLimit);

	switch (m_ResultType) {
		case ResultPerson:
			m_ListView->setSelectCallback(std::bind(&Chooser::onSelected, this, _1));
			break;
		case ResultNumber:
		case ResultEmail:
			if (m_SelectMode == SelectSingle) {
				m_ListView->setSelectCallback(std::bind(&Chooser::onSinglePersonSelected, this, _1));
			} else {
				m_ListView->setCheckCallback(std::bind(&Chooser::onPersonChecked, this, _1, _2));
				m_ListView->setSelectCallback(std::bind(&Chooser::onMultiPersonSelected, this, _1));
			}
			break;
		case ResultVcard:
			m_ListView->setSelectCallback(std::bind(&Chooser::onSelectedForVcard, this, _1));
			break;
		case ResultAction:
			m_ListView->setSelectCallback(std::bind(&Chooser::onSelectedForAction, this, _1));
			break;
		default:
			break;
	}

	navigateTo(m_ListView);
}

void Chooser::onPageAttached(Ui::NavigatorPage *page)
{
	/* FIXME: Disable naviframe item title by default */
	page->setTitle(nullptr);
}

bool Chooser::onPersonChecked(SelectItem *item, bool isChecked)
{
	if (isChecked) {
		return selectSingleResult(item->getSelectResult(), [this, item](SelectResults results) {
			item->setCustomResult(*results.begin());
			m_ListView->setCheckedItem(item, true);
			return true;
		});
	}

	item->unsetCustomResult();
	return true;
}

bool Chooser::onSinglePersonSelected(SelectResults results)
{
	return selectSingleResult(*results.begin(), std::bind(&Chooser::onSelected, this, _1));
}

bool Chooser::onMultiPersonSelected(SelectResults results)
{
	contacts_person_property_e property = (m_ResultType == ResultNumber)
			? CONTACTS_PERSON_PROPERTY_NUMBER
			: CONTACTS_PERSON_PROPERTY_EMAIL;

	for (auto &&result : results) {
		if (result.type != m_ResultType) {
			int id = 0;
			contacts_person_get_default_property(property, result.value.id, &id);
			result = { m_ResultType, id };
		}
	}

	return onSelected(results);
}

bool Chooser::onSelectedForAction(SelectResults results)
{
	return selectSingleResult(*results.begin(), [this](SelectResults results) {
		SelectResult result = *results.begin();
		if (result.type == ResultEmail) {
			result.type = ActionEmail;
			return onSelected({ &result, 1 });
		}

		Ui::ListPopup *popup = new Ui::ListPopup();
		popup->create(getEvasObject());
		popup->setTitle(getResultValue(result).c_str());
		popup->addItem("IDS_PB_OPT_VOICE_CALL", (void *) ActionCall);
		popup->addItem("IDS_PB_OPT_MESSAGE", (void *) ActionMessage);
		popup->setSelectedCallback([this, result] (void *data) mutable {
			result.type = (long) data;
			onSelected({ &result, 1 });
		});

		return false;
	});
}

bool Chooser::onSelectedForVcard(SelectResults results)
{
	using namespace Settings;

	std::vector<int> ids;
	ids.reserve(results.count());
	for (auto &&result : results) {
		ids.push_back(result.value.id);
	}

	auto exporter = new ExportController(getEvasObject(), "IDS_PB_HEADER_EXPORT_CONTACTS_ABB",
			std::move(ids), StorageInternalOther);
	exporter->setFinishCallback([this, exporter] {
		SelectResult result = { ResultVcard, (void *) exporter->getVcardPath().c_str() };
		onSelected({ &result, 1 });
	});
	exporter->run();

	return false;
}

bool Chooser::onSelected(SelectResults results)
{
	if (!m_OnSelected || m_OnSelected(results)) {
		getPage()->close();
	}

	return false;
}

bool Chooser::selectSingleResult(SelectResult person, SelectCallback callback)
{
	SelectResult result = getSingleResult(person.value.id);
	if (result.value.id > 0) {
		return callback({ &result, 1 });
	}

	DetailsView *view = new DetailsView(getDisplayContactId(person.value.id),
			DetailsView::Type(person.type), m_FilterType);
	view->setSelectMode(SelectSingle);
	view->setSelectCallback(callback);
	navigateTo(view);

	return false;
}

SelectResult Chooser::getSingleResult(int personId)
{
	int count = 0;

	int numberId = 0;
	if (m_FilterType & FilterNumber) {
		count += getResultCount(personId, ResultNumber, &numberId);
	}

	int emailId = 0;
	if (m_FilterType & FilterEmail) {
		count += getResultCount(personId, ResultEmail, &emailId);
	}

	SelectResult result = { 0, 0 };
	if (count == 1) {
		if (numberId) {
			result = { ResultNumber, numberId };
		} else if (emailId) {
			result = { ResultEmail, emailId };
		}
	}

	return result;
}

int Chooser::getResultCount(int personId, ResultType resultType, int *resultId)
{
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
			return 0;
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

		contacts_record_get_int(record, resultIdProp, resultId);
		contacts_list_destroy(list, true);
	}

	contacts_query_destroy(query);
	contacts_filter_destroy(filter);

	return count;
}

std::string Chooser::getResultValue(SelectResult result)
{
	std::string value;

	const char *uri = nullptr;
	int propId = 0;
	switch (result.type) {
		case ResultPerson:
			uri = _contacts_person._uri;
			propId = _contacts_person.display_name;
			break;
		case ResultNumber:
			uri = _contacts_number._uri;
			propId = _contacts_number.number;
			break;
		case ResultEmail:
			uri = _contacts_email._uri;
			propId = _contacts_email.email;
			break;
	}

	contacts_record_h record = nullptr;
	contacts_db_get_record(uri, result.value.id, &record);

	char *str = nullptr;
	contacts_record_get_str_p(record, propId, &str);
	if (str) {
		value = str;
	}
	contacts_record_destroy(record, true);

	return value;
}

int Chooser::getFilterType(ResultType resultType)
{
	switch (resultType) {
		case ResultNumber:
			return FilterNumber;
		case ResultEmail:
			return FilterEmail;
		case ResultAction:
			return FilterNumber | FilterEmail;
		default:
			return FilterNone;
	}
}
