/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef CONTACTS_RESULT_OBJECT_H
#define CONTACTS_RESULT_OBJECT_H

#include <contacts.h>
#include <tizen.h>

namespace Contacts
{
	class ResultObject;

	typedef std::unique_ptr<ResultObject> ResultPtr;
	typedef std::vector<ResultPtr> ResultList;

	class EXPORT_API ResultObject
	{
	public:
		virtual ~ResultObject() { }

		/**
		 * @return ID
		 */
		virtual int getId() = 0;

		/**
		 * @return name
		 */
		virtual const char *getName() = 0;

		/**
		 * @return number
		 */
		virtual const char *getNumber() = 0;

		/**
		 * @return image path
		 */
		virtual const char *getImagePath() = 0;

		/**
		 * @brief Determine if object searchable string contains @a str
		 * @return true if it contains, false if not
		 */
		bool compare(const char *str) = 0;
	};
}

#endif /* CONTACTS_RESULT_OBJECT_H */
