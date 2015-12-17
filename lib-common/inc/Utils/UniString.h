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
	class EXPORT_API UniString
	{
	public:
		UniString(const char *multibyteStr);
		UniString(const UniString &that);
		UniString(UniString &&that);
		~UniString();

		UniString &operator=(const UniString &that);
		UniString &operator=(UniString &&that);

		bool operator<(const UniString &that) const;
		bool operator>(const UniString &that) const;
		bool operator==(const UniString &that) const;
		bool operator!=(const UniString &that) const;

		const i18n_uchar *data() const;
		std::string toUtf8() const;

		size_t size() const;
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
