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

#include "Contacts/Model/ContactName.h"
#include "Contacts/Model/ContactTextField.h"
#include "Utils/Range.h"

#include <string.h>

using namespace Contacts::Model;

std::string ContactName::getValue() const
{
	unsigned fieldIds[] = { NameFieldTitle, NameFieldFirst, NameFieldMiddle, NameFieldLast };
	std::string value = getValues(fieldIds);

	auto suffixField = getFieldById<ContactTextField>(NameFieldSuffix);
	const char *suffix = suffixField->getValue();
	if (suffix) {
		if (!value.empty()) {
			value += ", ";
		}
		value += suffix;
	}

	return value;
}

void ContactName::setValue(std::string value)
{
	char *name = &value[0];
	const char *names[NameFieldEnd - NameFieldBegin] = { nullptr };

	names[NameFieldSuffix - NameFieldBegin] = splitSuffix(name);

	auto words = splitString(name);
	for (unsigned fieldId = NameFieldLast; fieldId > NameFieldFirst; --fieldId) {
		if (words.size() <= 1) {
			break;
		}

		names[fieldId - NameFieldBegin] = words.back();
		words.pop_back();
	}

	std::string firstName = mergeStrings(words);
	names[NameFieldFirst - NameFieldBegin] = firstName.c_str();

	setValues(names);
}

char *ContactName::splitSuffix(char *name)
{
	char *suffix = strchr(name, ',');
	if (suffix) {
		*suffix++ = '\0';
		suffix += strspn(suffix, " ,");
	}

	return suffix;
}
