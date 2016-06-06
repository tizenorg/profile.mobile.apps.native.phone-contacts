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

#include "Contacts/Model/ContactCompoundObject.h"
#include "Contacts/Model/ContactTextField.h"
#include "Utils/Logger.h"

#include <string.h>

using namespace Contacts::Model;

bool ContactCompoundObject::isEmpty() const
{
	return getValue().empty();
}

std::string ContactCompoundObject::getValues(Utils::Range<const unsigned *> fieldIds) const
{
	std::vector<const char *> values;
	for (auto &&fieldId : fieldIds) {
		auto field = getFieldById<ContactTextField>(fieldId);
		const char *value = field->getValue();
		values.push_back(value);
	}

	return mergeStrings(values);
}

void ContactCompoundObject::setValues(Utils::Range<const char **> values)
{
	for (size_t i = 0; i < values.count(); ++i) {
		ContactTextField *field = getField<ContactTextField>(i);
		if (values[i]) {
			field->setValue(values[i]);
		} else {
			field->reset();
		}
	}
}

std::vector<const char *> ContactCompoundObject::splitString(char *string)
{
	char *word_end = nullptr;
	char *word_begin = strtok_r(string, " ", &word_end);

	std::vector<const char *> words;
	while (word_begin) {
		words.push_back(word_begin);
		word_begin = strtok_r(nullptr, " ", &word_end);
	}

	return words;
}

std::string ContactCompoundObject::mergeStrings(const std::vector<const char *> &strings)
{
	std::string value;
	for (auto &&string : strings) {
		if (string && *string) {
			if (!value.empty()) {
				value += ' ';
			}
			value += string;
		}
	}

	return value;
}
