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

#ifndef _PH_DIALER_SEARCH_CONTROLLER_H_
#define _PH_DIALER_SEARCH_CONTROLLER_H_

#include <memory>
#include <Evas.h>

class PhDialerEntry;
class PhDialerView;

namespace Ph
{
	namespace Dialer
	{
		class PredictiveNumber;
		class SearchInfo;
	}
}

/**
 * @brief Predictive search result widget controller.
 */
class PhDialerSearchController
{
public:
	PhDialerSearchController();
	~PhDialerSearchController();

	/**
	 * @brief Creates predictive layout
	 * @param[in]	view	Parent view
	 */
	void create(PhDialerView &view);

private:
	enum class LayoutType
	{
		NONE,
		NO_CONTACTS,
		ONE_CONTACT,
		FEW_CONTACTS
	};

private:
	void onEntryChanged(PhDialerEntry &entry);

	void setLayout(const std::string &number);
	void setNoContactsLayout();
	void setLayoutWithContacts(const Ph::Dialer::SearchInfo &info);
	void setResultLayout(Evas_Object *parent, size_t count);

	void createSpeeddialLayout(Evas_Object *parent, const Ph::Dialer::SearchInfo &info);

	static void fillPredictiveLayout(Evas_Object *layout, const Ph::Dialer::SearchInfo &info);

	static void showResulListPopup(void *data, Evas *e, Evas_Object *obj, void *event_info);

	static void createAddToContactsPopup(void* data, Evas* e, Evas_Object* obj, void* event_info);

	static void onDbChanged(const char *view_uri, void *data);

	PhDialerView *m_View;

	std::unique_ptr<Ph::Dialer::PredictiveNumber> m_PredictiveNumber;

	LayoutType m_CurrentLayoutType;
	Evas_Object *m_Layout;
	Evas_Object *m_ResultLayout;
};

#endif //_PH_DIALER_SEARCH_CONTROLLER_H_
