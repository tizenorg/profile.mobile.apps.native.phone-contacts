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

#include "PhDialerSearchController.h"
#include "PhDialerAddToContactsPopup.h"
#include "PhDialerResultListPopup.h"
#include "PhDialerView.h"
#include "PhDialerPredictiveNumber.h"
#include "PhDialerPredictiveNumberUtils.h"
#include "PhDialerLayout.h"
#include "PhDialerEntry.h"
#include "PhPath.h"
#include "ContactsDebug.h"
#include "ContactsLocalization.h"

using namespace Ph::Dialer;

namespace
{
	const std::string layoutFilePath = ContactsCommon::getResourcePath(DIALER_PREDICTIVE_EDJ);
}

PhDialerSearchController::PhDialerSearchController()
	: m_View(nullptr),
	  m_PredictiveNumber(new PredictiveNumber()),
	  m_CurrentLayoutType(LayoutType::NONE),
	  m_Layout(nullptr),
	  m_ResultLayout(nullptr)
{
	int err = CONTACTS_ERROR_NONE;
	err = contacts_db_add_changed_cb(_contacts_contact._uri, &PhDialerSearchController::onDbChanged, this);
	WPWARN(CONTACTS_ERROR_NONE != err,"contacts_db_add_changed_cb() Failed(%d)", err);
}

PhDialerSearchController::~PhDialerSearchController()
{
	int err = CONTACTS_ERROR_NONE;
	err = contacts_db_remove_changed_cb(_contacts_contact._uri, &PhDialerSearchController::onDbChanged, this);
	WPWARN(CONTACTS_ERROR_NONE != err,"contacts_db_add_changed_cb() Failed(%d)", err);
}

void PhDialerSearchController::create(PhDialerView &view)
{
	m_View = &view;
	PhDialerEntry *entry = m_View->getEntry();

	entry->setOnChanged(std::bind(&PhDialerSearchController::onEntryChanged, this, std::placeholders::_1));
}

void PhDialerSearchController::setLayout(const std::string &number)
{
	if(number.empty()) {
		m_Layout = nullptr;
		m_CurrentLayoutType = LayoutType::NONE;
		m_View->setPredictiveLayout(nullptr);
	} else {
		if(m_PredictiveNumber->empty()) {
			setNoContactsLayout();
		} else {
			const SearchList &result = *m_PredictiveNumber->getSearchResult();
			setLayoutWithContacts(*result.front());
			setResultLayout(m_Layout, result.size());
		}
	}
}

void PhDialerSearchController::onEntryChanged(PhDialerEntry &entry)
{
	std::string number(entry.getNumber());
	m_PredictiveNumber->search(number);

	setLayout(number);
}

void PhDialerSearchController::setNoContactsLayout()
{
	if(m_CurrentLayoutType == LayoutType::NO_CONTACTS) {
		return;
	}
	m_CurrentLayoutType = LayoutType::NO_CONTACTS;

	m_Layout = elm_layout_add(m_View->getEvasObj());
	elm_layout_file_set(m_Layout, layoutFilePath.c_str(), GROUP_PREDICTIVE_NO_RESULTS);
	elm_object_part_text_set(m_Layout, PART_TEXT_ADD, "IDS_KPD_BUTTON_ADD_TO_CONTACTS_ABB2");
	elm_object_domain_part_text_translatable_set(m_Layout, PART_TEXT_ADD, TEXT_DOMAIN, true);

	evas_object_event_callback_add(m_Layout, EVAS_CALLBACK_MOUSE_UP, &createAddToContactsPopup, this);

	m_View->setPredictiveLayout(m_Layout);
}

void PhDialerSearchController::setLayoutWithContacts(const Ph::Dialer::SearchInfo &info)
{
	if(m_CurrentLayoutType != LayoutType::ONE_CONTACT
		&& m_CurrentLayoutType != LayoutType::FEW_CONTACTS) {

		m_CurrentLayoutType = LayoutType::ONE_CONTACT;

		m_Layout = elm_layout_add(m_View->getEvasObj());
		elm_layout_file_set(m_Layout, layoutFilePath.c_str(), GROUP_PREDICTIVE);

		evas_object_event_callback_add(m_Layout, EVAS_CALLBACK_MOUSE_UP,
			[](void *data, Evas *e, Evas_Object *obj, void *event_info) {
				PhDialerSearchController *self = (PhDialerSearchController*)data;
				const SearchList *result = self->m_PredictiveNumber->getSearchResult();
				if (result) {
					SearchInfo &info = *result->front();
					self->m_View->getEntry()->setNumber(info.getNumber(false));
				}
			},
		this);

		m_View->setPredictiveLayout(m_Layout);
	}

	if(info.getType() == IT_SPEED_DIAL) {
		createSpeeddialLayout(m_Layout, info);
		elm_object_signal_emit(m_Layout, "show,speeddial,icon", "");
	} else {
		elm_object_signal_emit(m_Layout, "hide,speeddial,icon", "");
	}

	fillPredictiveLayout(m_Layout, info);
}

void PhDialerSearchController::setResultLayout(Evas_Object *parent, size_t count)
{
	if(count > 1) {
		if(m_CurrentLayoutType != LayoutType::FEW_CONTACTS) {
			m_CurrentLayoutType = LayoutType::FEW_CONTACTS;

			m_ResultLayout = elm_layout_add(parent);
			elm_layout_file_set(m_ResultLayout, layoutFilePath.c_str(), GROUP_PREDICTIVE_RES_COUNT);

			evas_object_event_callback_add(m_ResultLayout, EVAS_CALLBACK_MOUSE_UP, &showResulListPopup, this);
			evas_object_propagate_events_set(m_ResultLayout, EINA_FALSE);
		}

		elm_object_part_text_set(m_ResultLayout, PART_TEXT_COUNT, std::to_string(count).c_str());
	} else {
		m_ResultLayout = nullptr;
		m_CurrentLayoutType = LayoutType::ONE_CONTACT;
	}
	elm_object_part_content_set(parent, PART_SWALLOW_RESULTS, m_ResultLayout);
}

void PhDialerSearchController::createSpeeddialLayout(Evas_Object *parent, const Ph::Dialer::SearchInfo &info)
{
	Evas_Object *speeddialLayout = elm_layout_add(parent);
	elm_layout_file_set(speeddialLayout, layoutFilePath.c_str(), GROUP_SPEEDDIAL_NUMBER);
	elm_object_part_text_set(speeddialLayout, PART_TEXT_NUMBER, info.getSearchString().c_str());

	elm_object_part_content_set(parent, PART_SWALLOW_SPEEDDIAL, speeddialLayout);
}

void PhDialerSearchController::fillPredictiveLayout(Evas_Object *layout, const Ph::Dialer::SearchInfo &info)
{
	elm_object_part_content_set(layout, PART_SWALLOW_THUMBNAIL,
			Utils::createThumbnail(layout, info.getId()));

	if(info.getName(false).empty()) {
		elm_object_part_text_set(layout, PART_TEXT_1_LINE, info.getNumber(true).c_str());
		elm_object_part_text_set(layout, PART_TEXT_NAME, "");
		elm_object_part_text_set(layout, PART_TEXT_NUMBER, "");
	} else {
		elm_object_part_text_set(layout, PART_TEXT_1_LINE, "");
		elm_object_part_text_set(layout, PART_TEXT_NAME, info.getName(true).c_str());
		elm_object_part_text_set(layout, PART_TEXT_NUMBER, info.getNumber(true).c_str());
	}
}

void PhDialerSearchController::showResulListPopup(void* data, Evas* e,
		Evas_Object* obj, void* event_info)
{
	PhDialerSearchController *searchController = (PhDialerSearchController*)data;
	searchController->m_View->attachPopup(
			new PhDialerResultListPopup(*searchController->m_PredictiveNumber->getSearchResult(), *searchController->m_View->getEntry()));
}

void PhDialerSearchController::createAddToContactsPopup(void* data, Evas* e,
		Evas_Object* obj, void* event_info)
{
	WPRET_M(!data, "data = NULL");
	PhDialerSearchController *searchController = (PhDialerSearchController*)data;
	searchController->m_View->attachPopup(new PhDialerAddToContactsPopup(searchController->m_View->getEntry()->getNumber()));
}

void PhDialerSearchController::onDbChanged(const char *view_uri, void *data)
{
	PhDialerSearchController* controller = (PhDialerSearchController*) data;
	std::string number(controller->m_View->getEntry()->getNumber());
	controller->m_PredictiveNumber->searchFromScratch(number);
	controller->setLayout(number);
}
