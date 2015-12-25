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

#include "Contacts/Model/ContactPhoneticName.h"
#include "Contacts/Model/ContactTextField.h"
#include "Utils/Range.h"

using namespace Contacts::Model;

std::string ContactPhoneticName::getValue() const
{
	unsigned fieldIds[] = { PhoneticNameFieldFirst, PhoneticNameFieldMiddle, PhoneticNameFieldLast };
	return getValues(fieldIds);
}

void ContactPhoneticName::setValue(std::string value)
{
	char *name = &value[0];
	const char *names[PhoneticNameFieldEnd - PhoneticNameFieldBegin] = { nullptr };

	auto words = splitString(name);
	for (unsigned fieldId = PhoneticNameFieldLast; fieldId > PhoneticNameFieldFirst; --fieldId) {
		if (words.size() <= 1) {
			break;
		}

		names[fieldId - PhoneticNameFieldBegin] = words.back();
		words.pop_back();
	}

	std::string firstName = mergeStrings(words);
	names[PhoneticNameFieldFirst - PhoneticNameFieldBegin] = firstName.c_str();

	setValues(names);
}
