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

#include <algorithm>
#include <contacts.h>
#include <account.h>
#include <app_preference.h>
#include "ContactsDebug.h"
#include "CtSettingData.h"
#include "CtType.h"
#include "CtString.h"

CtSettingData::CtSettingData()
: __sortMode(SORT_MODE_FIRST_NAME)
, __nameOrder(NAME_ORDER_LAST_NAME_FIRST)
{
	WHIT();
}

CtSettingData::~CtSettingData()
{
	WHIT();
}

void CtSettingData::init()
{
	// Load data from preference

	contacts_name_sorting_order_e sortOrder = CONTACTS_NAME_SORTING_ORDER_FIRSTLAST;
	contacts_setting_get_name_sorting_order(&sortOrder);
	if (sortOrder == CONTACTS_NAME_SORTING_ORDER_FIRSTLAST)
		__sortMode = SORT_MODE_FIRST_NAME;
	else
		__sortMode = SORT_MODE_LAST_NAME;

	contacts_name_display_order_e displayOrder = CONTACTS_NAME_DISPLAY_ORDER_FIRSTLAST;
	contacts_setting_get_name_display_order(&displayOrder);
	if (displayOrder == CONTACTS_NAME_DISPLAY_ORDER_FIRSTLAST)
		__nameOrder = NAME_ORDER_FIRST_NAME_FIRST;
	else
		__nameOrder = NAME_ORDER_LAST_NAME_FIRST;
}

CtSettingData::SortMode CtSettingData::getSortMode() const
{
	return __sortMode;
}

void CtSettingData::setSortMode(SortMode sortMode)
{
	if (sortMode == SORT_MODE_FIRST_NAME)
		contacts_setting_set_name_sorting_order(CONTACTS_NAME_SORTING_ORDER_FIRSTLAST);
	else
		contacts_setting_set_name_sorting_order(CONTACTS_NAME_SORTING_ORDER_LASTFIRST);

	__sortMode = sortMode;
}

CtSettingData::NameOrder CtSettingData::getNameOrder() const
{
	return __nameOrder;
}

void CtSettingData::setNameOrder(NameOrder nameOrder)
{
	if (nameOrder == NAME_ORDER_FIRST_NAME_FIRST)
		contacts_setting_set_name_display_order(CONTACTS_NAME_DISPLAY_ORDER_FIRSTLAST);
	else
		contacts_setting_set_name_display_order(CONTACTS_NAME_DISPLAY_ORDER_LASTFIRST);

	__nameOrder = nameOrder;
}
