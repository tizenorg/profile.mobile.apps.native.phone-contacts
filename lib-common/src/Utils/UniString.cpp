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
	: m_Utf8Str(utf8Str)
{
	m_UniStr = fromUtf8(m_Utf8Str);
}

bool UniString::operator<(const UniString &that) const
{
	return i18n_ustring_compare_code_point_order(m_UniStr.c_str(), that.m_UniStr.c_str()) < 0;
}

bool UniString::operator>(const UniString &that) const
{
	return i18n_ustring_compare_code_point_order(m_UniStr.c_str(), that.m_UniStr.c_str()) > 0;
}

bool UniString::operator==(const UniString &that) const
{
	return i18n_ustring_compare_code_point_order(m_UniStr.c_str(), that.m_UniStr.c_str()) == 0;
}

bool UniString::operator!=(const UniString &that) const
{
	return i18n_ustring_compare_code_point_order(m_UniStr.c_str(), that.m_UniStr.c_str()) != 0;
}

bool UniString::operator<=(const UniString &that) const
{
	return i18n_ustring_compare_code_point_order(m_UniStr.c_str(), that.m_UniStr.c_str()) <= 0;
}

bool UniString::operator>=(const UniString &that) const
{
	return i18n_ustring_compare_code_point_order(m_UniStr.c_str(), that.m_UniStr.c_str()) >= 0;
}

const UniString::I18nString &UniString::getI18nStr() const
{
	return m_UniStr;
}

const std::string &UniString::getUtf8Str() const
{
	if (!m_Utf8Str.length() && m_UniStr.length()) {
		m_Utf8Str = toUtf8(m_UniStr);
	}

	return m_Utf8Str;
}

std::string UniString::toUtf8(const I18nString &ustring) const
{
	char fakeValue;
	int length = 0;
	i18n_error_code_e err = I18N_ERROR_NONE;
	i18n_ustring_to_UTF8(&fakeValue, 1, &length, ustring.c_str(), -1, &err);

	std::unique_ptr<char> retVal(new char[length + 1]);
	i18n_ustring_to_UTF8(retVal.get(), length + 1, &length, ustring.c_str(), -1, &err);

	return retVal.get();
}

UniString::I18nString UniString::fromUtf8(const std::string& utf8Str) const
{
	i18n_uchar fakeValue;
	int length = 0;
	i18n_error_code_e err = I18N_ERROR_NONE;
	i18n_ustring_from_UTF8(&fakeValue, 1, &length, utf8Str.c_str(), -1, &err);

	std::unique_ptr<i18n_uchar> retVal(new i18n_uchar[length + 1]);
	i18n_ustring_from_UTF8(retVal.get(), length + 1, &length, utf8Str.c_str(), -1, &err);

	return retVal.get();
}
