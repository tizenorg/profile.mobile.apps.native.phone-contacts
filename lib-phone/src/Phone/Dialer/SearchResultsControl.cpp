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

#include "Phone/Dialer/SearchResultsControl.h"
#include "Phone/Dialer/SearchResultsPopup.h"
#include "Phone/Dialer/SearchUtils.h"

#include "App/Path.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include "DialerLayout.h"

using namespace Phone::Dialer;

namespace
{
	const std::string layoutFilePath = App::getResourcePath(DIALER_PREDICTIVE_EDJ);
}

/* FIXME: Added for 2.4 SDK build */
extern "C" Eina_List *elm_layout_content_swallow_list_get(Evas_Object *obj);

SearchResultsControl::SearchResultsControl()
	: m_Results(nullptr), m_State(ResultsNone),
	  m_ResultsCount(nullptr)
{
}

void SearchResultsControl::setResults(const SearchResults *results)
{
	m_Results = results;
	if (!m_Results || m_Results->empty()) {
		setResultsEmpty();
	} else {
		setResultsPresent();
		setResultsCount(m_Results->size());
	}
}

void SearchResultsControl::clearResults()
{
	setLayout(nullptr);
	m_State = ResultsNone;
	m_Results = nullptr;
}

void SearchResultsControl::setSelectedCallback(SelectedCallback callback)
{
	m_OnSelected = std::move(callback);
}

Evas_Object *SearchResultsControl::onCreate(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	evas_object_event_callback_add(layout, EVAS_CALLBACK_MOUSE_DOWN,
			(Evas_Object_Event_Cb) makeCallback(&SearchResultsControl::onResultPressed), this);
	return layout;
}

void SearchResultsControl::setLayout(const char *groupName)
{
	clearLayout();
	elm_layout_file_set(getEvasObject(), layoutFilePath.c_str(), groupName);
}

void SearchResultsControl::clearLayout()
{
	Eina_List *list = elm_layout_content_swallow_list_get(getEvasObject());
	Eina_List *node = nullptr;
	void *obj = nullptr;
	EINA_LIST_FOREACH(list, node, obj) {
		evas_object_del((Evas_Object *) obj);
	}

	eina_list_free(list);
}

void SearchResultsControl::setResultsEmpty()
{
	if (m_State != ResultsEmpty) {
		m_State = ResultsEmpty;
		setLayout(GROUP_PREDICTIVE_NO_RESULTS);
		elm_object_translatable_part_text_set(getEvasObject(), PART_TEXT_ADD, "IDS_KPD_BUTTON_ADD_TO_CONTACTS_ABB2");
	}
}

void SearchResultsControl::setResultsPresent()
{
	if (m_State != ResultsPresent && m_State != ResultsMany) {
		m_State = ResultsPresent;
		setLayout(GROUP_PREDICTIVE);
	}

	setResultInfo(m_Results->front());
}

void SearchResultsControl::setResultsCount(size_t count)
{
	if (count > 1) {
		if (m_State != ResultsMany) {
			m_State = ResultsMany;

			m_ResultsCount = elm_layout_add(getEvasObject());
			elm_layout_file_set(m_ResultsCount, layoutFilePath.c_str(), GROUP_PREDICTIVE_RES_COUNT);
			evas_object_propagate_events_set(m_ResultsCount, EINA_FALSE);
			evas_object_event_callback_add(m_ResultsCount, EVAS_CALLBACK_MOUSE_DOWN,
					(Evas_Object_Event_Cb) makeCallback(&SearchResultsControl::onShowResultsPressed), this);
		}

		elm_object_part_text_set(m_ResultsCount, PART_TEXT_COUNT,
				std::to_string(count).c_str());
	} else {
		m_ResultsCount = nullptr;
		m_State = ResultsPresent;
	}

	elm_object_part_content_set(getEvasObject(), PART_SWALLOW_RESULTS, m_ResultsCount);
}

void SearchResultsControl::setResultInfo(SearchResultPtr result)
{
	Evas_Object *layout = getEvasObject();
	elm_object_part_content_set(layout, PART_SWALLOW_THUMBNAIL,
			Utils::createThumbnail(layout, result->getId()));

	if (result->getName(false).empty()) {
		elm_object_part_text_set(layout, PART_TEXT_1_LINE, result->getNumber(true).c_str());
		elm_object_part_text_set(layout, PART_TEXT_NAME, "");
		elm_object_part_text_set(layout, PART_TEXT_NUMBER, "");
	} else {
		elm_object_part_text_set(layout, PART_TEXT_1_LINE, "");
		elm_object_part_text_set(layout, PART_TEXT_NAME, result->getName(true).c_str());
		elm_object_part_text_set(layout, PART_TEXT_NUMBER, result->getNumber(true).c_str());
	}

	if (result->getType() == ResultSpeedDial) {
		setResultSpeedDial(result);
		elm_object_signal_emit(getEvasObject(), "show,speeddial,icon", "");
	} else {
		elm_object_signal_emit(getEvasObject(), "hide,speeddial,icon", "");
	}
}

void SearchResultsControl::setResultSpeedDial(SearchResultPtr result)
{
	Evas_Object *speeddialLayout = elm_layout_add(getEvasObject());
	elm_layout_file_set(speeddialLayout, layoutFilePath.c_str(), GROUP_SPEEDDIAL_NUMBER);
	elm_object_part_text_set(speeddialLayout, PART_TEXT_NUMBER, result->getSearchString().c_str());
	elm_object_part_content_set(getEvasObject(), PART_SWALLOW_SPEEDDIAL, speeddialLayout);
}

void SearchResultsControl::onResultPressed()
{
	if (m_OnSelected) {
		if (m_Results && !m_Results->empty()) {
			m_OnSelected(m_Results->front());
		} else {
			m_OnSelected(nullptr);
		}
	}
}

void SearchResultsControl::onShowResultsPressed()
{
	SearchResultsPopup *popup = new SearchResultsPopup(m_Results);
	popup->setSelectedCallback(m_OnSelected);
	popup->create(getEvasObject());
}
