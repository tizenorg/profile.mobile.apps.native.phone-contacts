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

#ifndef COMMON_STRINGS_H
#define COMMON_STRINGS_H

#include "Common/DataTypes.h"
#include "Utils/Range.h"
#include "Ux/SelectView.h"

#include <tizen.h>

namespace Common
{
	/**
	 * @brief Contact enum value-name pair.
	 * @see EnumType
	 */
	struct EnumValueNamePair
	{
		int value;          /**< Enum value */
		const char *name;   /**< Translatable value name */
	};

	/**
	 * @brief Get contact enum value names.
	 * @param[in]   type    Enum type
	 * @return Enum value translatable names range.
	 */
	EXPORT_API Utils::Range<const EnumValueNamePair *> getEnumValueNames(EnumType type);

	/**
	 * @brief Get translatable name for enum value.
	 * @param[in]   type    Enum type
	 * @param[in]   value   Enum value
	 * @return Enum value translatable name.
	 */
	EXPORT_API const char *getEnumValueName(EnumType type, int value);

	/**
	 * @return Default translatable strings table for Ux::SelectView.
	 */
	EXPORT_API const Ux::SelectView::Strings &getSelectViewStrings();
}

#endif /* COMMON_STRINGS_H */
