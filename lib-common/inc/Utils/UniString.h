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
		 * @brief Create new object, based on @a utf8Str string
		 * @param[in]   utf8Str     UTF8 multibyte unicode string
		 */
		UniString(const char *utf8Str);
		UniString(const UniString &that);
		UniString(UniString &&that);
		~UniString();

		UniString &operator=(const UniString &that);
		UniString &operator=(UniString &&that);

		bool operator<(const UniString &that) const;
		bool operator>(const UniString &that) const;
		bool operator==(const UniString &that) const;
		bool operator!=(const UniString &that) const;
		bool operator<=(const UniString &that) const;
		bool operator>=(const UniString &that) const;

		/**
		 * @see utils_i18n.h header
		 * @return Pointer to null-terminated sequence of i18n_uchar characters
		 */
		const i18n_uchar *data() const;

		/**
		 * @return Utf8 multibyte unicode string
		 */
		std::string toUtf8() const;

		/**
		 * @return String size in bytes, including null-terminate character
		 */
		size_t size() const;

		/**
		 * @return Character count, not including null-terminate character
		 */
		size_t length() const;

	private:
		void copy(const UniString &that);
		void move(UniString &that);
		void clear();
		void nullify();

		i18n_uchar *m_String;
		size_t m_Length;
	};
}

#endif /* UTILS_UNI_STRING_H */
