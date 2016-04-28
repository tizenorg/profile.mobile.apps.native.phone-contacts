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

#include "Contacts/List/PersonItem.h"
#include "Contacts/List/Model/Person.h"
#include "Contacts/Common/ContactSelectTypes.h"
#include "Contacts/Details/DetailsView.h"

#include "Ui/Genlist.h"
#include "Ui/Navigator.h"

using namespace Contacts::List;
using namespace Contacts::List::Model;

PersonItem::PersonItem(Person &person)
	: ContactItem(person)
{
}

Person &PersonItem::getPerson() const
{
	return static_cast<Person &>(getContactData());
}

Ux::SelectResult PersonItem::getDefaultResult() const
{
	return { ResultPerson, getPerson().getId() };
}

void PersonItem::onSelected()
{
	if (getSelectMode() != Ux::SelectNone) {
		SelectItem::onSelected();
		return;
	}

	Ui::Navigator *navigator = getParent()->findParent<Ui::Navigator>();
	if (!navigator) {
		return;
	}

	int id = getPerson().getContactId();
	navigator->navigateTo(new Details::DetailsView(id));
}
