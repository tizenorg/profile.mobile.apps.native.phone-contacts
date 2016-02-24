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

#ifndef UTILS_UNI_STRING_H
#define UTILS_UNI_STRING_H

#include <utils_i18n.h>
#include <string>

namespace Utils
{
	/**
	 * @brief Class wrapper for convenient work with multibyte UTF8 unicode string
	 */
	class EXPORT_API UniString
	{
	public:
		/**
		 * @brief Instantiation of the basic_string class template,
		 * that uses i18n_uchar as its character type
		 */
		typedef std::basic_string<i18n_uchar> I18nString;

		UniString() = default;
		/**
		 * @brief Create new object, based on @a utf8Str string
		 * @param[in]   utf8Str     UTF8 multibyte unicode string
		 */
		UniString(const char *utf8Str);

		bool operator<(const UniString &that) const;
		bool operator>(const UniString &that) const;
		bool operator==(const UniString &that) const;
		bool operator!=(const UniString &that) const;
		bool operator<=(const UniString &that) const;
		bool operator>=(const UniString &that) const;

		/**
		 * @return I18n string
		 */
		const I18nString &getI18nStr() const;

		/**
		 * @return UTF8 multibyte string
		 */
		const std::string &getUtf8Str() const;

	private:
		static std::string toUtf8(const I18nString &ustring);
		static I18nString fromUtf8(const std::string &utf8Str);

		mutable std::string m_Utf8Str;
		I18nString m_UniStr;
	};
}

#endif /* UTILS_UNI_STRING_H */
