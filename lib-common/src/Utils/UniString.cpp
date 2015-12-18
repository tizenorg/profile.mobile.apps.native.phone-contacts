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

#include "Utils/UniString.h"
#include <cstring>
#include <memory>

using namespace Utils;

UniString::UniString(const char *utf8Str)
	: m_String(nullptr), m_Length(0)
{
	i18n_uchar fakeValue;
	int length = 0;
	i18n_error_code_e err = I18N_ERROR_NONE;
	i18n_ustring_from_UTF8(&fakeValue, 1, &length, utf8Str, -1, &err);

	m_String = new i18n_uchar[length + 1];
	i18n_ustring_from_UTF8(m_String, length + 1, &length, utf8Str, -1, &err);

	m_Length = length;
}

UniString::UniString(const UniString &that)
{
	copy(that);
}

UniString::UniString(UniString &&that)
{
	move(that);
}

UniString::~UniString()
{
	clear();
}

UniString &UniString::operator=(const UniString &that)
{
	if (this != &that) {
		clear();
		copy(that);
	}

	return *this;
}

UniString &UniString::operator=(UniString &&that)
{
	if (this != &that) {
		clear();
		move(that);
	}

	return *this;
}

bool UniString::operator<(const UniString &that) const
{
	return i18n_ustring_compare_code_point_order(m_String, that.m_String) < 0;
}

bool UniString::operator>(const UniString &that) const
{
	return i18n_ustring_compare_code_point_order(m_String, that.m_String) > 0;
}

bool UniString::operator==(const UniString &that) const
{
	return i18n_ustring_compare_code_point_order(m_String, that.m_String) == 0;
}

bool UniString::operator!=(const UniString &that) const
{
	return i18n_ustring_compare_code_point_order(m_String, that.m_String) != 0;
}

bool UniString::operator<=(const UniString &that) const
{
	return i18n_ustring_compare_code_point_order(m_String, that.m_String) <= 0;
}

bool UniString::operator>=(const UniString &that) const
{
	return i18n_ustring_compare_code_point_order(m_String, that.m_String) >= 0;
}

const i18n_uchar *UniString::data() const
{
	return m_String;
}
std::string UniString::toUtf8() const
{
	char fakeValue;
	int length = 0;
	i18n_error_code_e err = I18N_ERROR_NONE;
	i18n_ustring_to_UTF8(&fakeValue, 1, &length, m_String, -1, &err);

	std::unique_ptr<char> retVal(new char[length + 1]);
	i18n_ustring_to_UTF8(retVal.get(), length + 1, &length, m_String, -1, &err);

	return retVal.get();
}

size_t UniString::size() const
{
	return (m_Length + 1) * sizeof(i18n_uchar);
}

size_t UniString::length() const
{
	return m_Length;
}

void UniString::copy(const UniString &that)
{
	m_String = new i18n_uchar[that.size()];
	memcpy(m_String, that.m_String, that.size());
	m_Length = that.m_Length;
}

void UniString::move(UniString &that)
{
	m_String = that.m_String;
	m_Length = that.m_Length;
	that.nullify();
}

void UniString::clear()
{
	delete[] m_String;
	nullify();
}

void UniString::nullify()
{
	m_String = nullptr;
	m_Length = 0;
}
