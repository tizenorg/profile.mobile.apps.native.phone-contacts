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

#include "Contacts/Model/ContactFieldIterator.h"
#include "Contacts/Model/ContactFieldContainer.h"

using namespace Contacts::Model;

ContactFieldIterator::ContactFieldIterator(const ContactFieldContainer &container, int index)
	: m_Container(container), m_Index(index)
{
}

ContactFieldIterator &ContactFieldIterator::operator++()
{
	++m_Index;
	return *this;
}

ContactField &ContactFieldIterator::operator*() const
{
	return *m_Container.getField(m_Index);
}

bool ContactFieldIterator::operator!=(const ContactFieldIterator &that) const
{
	return m_Index != that.m_Index;
}
