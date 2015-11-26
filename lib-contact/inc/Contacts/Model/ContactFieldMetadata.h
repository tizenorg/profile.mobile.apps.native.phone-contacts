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

#ifndef CONTACTS_MODEL_CONTACT_METADATA_H
#define CONTACTS_MODEL_CONTACT_METADATA_H

#include "Contacts/Model/ContactTypes.h"

namespace Contacts
{
	namespace Model
	{
		/**
		 * @brief Contact field type metadata for runtime identification.
		 *
		 * @see ContactFieldType
		 * @see ContactObjectType
		 * @see ContactEnumType
		 * @see ContactTextType
		 */
		struct ContactFieldMetadata
		{
			ContactFieldType type; /**< Field basic type */
			unsigned subType;      /**< Field subtype depending on the type (ContactObjectType, ContactEnumType or ContactTextType) */
		};

		/**
		 * @brief Array of objects metadata (object type specified by ContactObjectType in subType)
		 */
		struct ContactArrayMetadata
		{
			ContactFieldMetadata base;           /**< C-style base to allow list initialization without constructor */
			const ContactFieldMetadata *element; /**< Array element metadata */
		};

		/**
		 * @brief Object field
		 *
		 * @see ContactObjectMetadata
		 */
		struct ContactObjectField
		{
			unsigned id; /**< Field ID */
			const ContactFieldMetadata *metadata; /**< Field metadata */
		};

		/**
		 * @brief Object metadata
		 */
		struct ContactObjectMetadata
		{
			ContactFieldMetadata base;        /**< C-style base */
			const ContactObjectField *fields; /**< Field that belong to the object */
			unsigned fieldCount;              /**< Object fields count */
		};

		/**
		 * @brief Enum type metadata
		 *
		 * @see ContactFieldMetadata
		 */
		struct ContactEnumMetadata
		{
			ContactFieldMetadata base; /**< C-style base */
			const int *values;         /**< Possible values */
			unsigned valueCount;       /**< Possible value count */
			int defaultValue;          /**< Default value */
			int customValue;           /**< Value that signifies the custom value being set */
		};

		/**
		 * @brief Get contact field metadata by type.
		 * @param[in]   fieldType       Field type
		 * @param[in]   fieldSubType    Field subtype
		 * @return Field metadata.
		 *
		 * @see ContactFieldType
		 * @see ContactObjectType
		 * @see ContactEnumType
		 * @see ContactTextType
		 */
		const ContactFieldMetadata *getContactFieldMetadata(ContactFieldType fieldType, unsigned fieldSubType);
	}
}

#endif /* CONTACTS_MODEL_CONTACT_METADATA_H */
