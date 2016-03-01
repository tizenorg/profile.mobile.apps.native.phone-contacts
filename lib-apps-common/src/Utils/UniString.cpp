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
{
	if (utf8Str) {
		m_Utf8Str = utf8Str;
	}
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

UniString::I18nString &UniString::getI18nStr()
{
	return m_UniStr;
}

std::string &UniString::getUtf8Str()
{
	if (m_Utf8Str.empty() && !m_UniStr.empty()) {
		m_Utf8Str = toUtf8(m_UniStr);
	}

	return m_Utf8Str;
}

void UniString::clear()
{
	m_Utf8Str.clear();
	m_UniStr.clear();
}

std::string UniString::toUtf8(const I18nString &ustring)
{
	char fakeValue;
	int length = 0;
	i18n_error_code_e err = I18N_ERROR_NONE;
	i18n_ustring_to_UTF8(&fakeValue, 1, &length, ustring.c_str(), -1, &err);

	std::string retVal(length + 1, 0);
	i18n_ustring_to_UTF8(&retVal[0], length + 1, &length, ustring.c_str(), -1, &err);

	return retVal;
}

UniString::I18nString UniString::fromUtf8(const std::string &utf8Str)
{
	i18n_uchar fakeValue;
	int length = 0;
	i18n_error_code_e err = I18N_ERROR_NONE;
	i18n_ustring_from_UTF8(&fakeValue, 1, &length, utf8Str.c_str(), -1, &err);

	I18nString retVal(length + 1, 0);
	i18n_ustring_from_UTF8(&retVal[0], length + 1, &length, utf8Str.c_str(), -1, &err);

	return retVal;
}
