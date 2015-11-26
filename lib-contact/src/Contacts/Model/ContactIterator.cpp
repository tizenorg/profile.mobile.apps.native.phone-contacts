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

#include "Contacts/Model/ContactIterator.h"
#include "Contacts/Model/ContactArray.h"
#include "Contacts/Model/ContactObject.h"

using namespace Contacts::Model;

ContactIterator::ContactIterator(const ContactField *field, int index)
	: m_Index(index), m_Field(field)
{
}

ContactIterator &ContactIterator::operator++()
{
	++m_Index;
	return *this;
}

bool ContactIterator::operator!=(const ContactIterator &that) const
{
	return m_Index != that.m_Index;
}

ContactFieldPtr ContactArrayIterator::operator*() const
{
	return static_cast<const ContactArray *>(m_Field)->getField(m_Index);
}

ContactFieldPtr ContactObjectIterator::operator*() const
{
	return static_cast<const ContactObject *>(m_Field)->getFieldByIndex(m_Index);
}
