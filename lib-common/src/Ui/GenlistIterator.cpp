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

#include "Ui/GenlistIterator.h"
#include "Ui/GenlistItem.h"

using namespace Ui;

GenlistIterator::GenlistIterator(GenlistItem* item)
	: m_Item(item)
{
}

GenlistIterator &GenlistIterator::operator++()
{
	if (m_Item) {
		m_Item = m_Item->getNextItem();
	}
	return *this;
}

GenlistIterator GenlistIterator::operator++(int)
{
	auto it(*this);
	++*this;
	return it;
}

GenlistIterator &GenlistIterator::operator--()
{
	if (m_Item) {
		m_Item = m_Item->getPrevItem();
	}
	return *this;
}

GenlistIterator GenlistIterator::operator--(int)
{
	auto it(*this);
	--*this;
	return it;
}

GenlistItem *GenlistIterator::operator*() const
{
	return m_Item;
}

bool GenlistIterator::operator!=(const GenlistIterator &that) const
{
	return m_Item != that.m_Item;
}
