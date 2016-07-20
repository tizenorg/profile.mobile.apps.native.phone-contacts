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

#ifndef CONTACTS_MODEL_CONTACT_METADATA_H
#define CONTACTS_MODEL_CONTACT_METADATA_H

#include "Contacts/Model/ContactTypes.h"
#include "Utils/Range.h"

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
		struct ContactTypeMetadata
		{
			ContactFieldType type; /**< Field basic type */
			unsigned subType;      /**< Field subtype depending on the type
			                            (ContactObjectType, ContactEnumType or ContactTextType) */
		};

		/**
		 * @brief Contact field metadata.
		 *
		 * @see ContactObjectMetadata
		 */
		struct ContactFieldMetadata
		{
			unsigned id;     /**< Field ID */
			unsigned propId; /**< Record property ID */
			bool isRequired; /**< Whether the field determines that parent object is filled */
			const ContactTypeMetadata *typeMetadata; /**< Field type metadata */
		};

		/**
		 * @brief Object metadata.
		 */
		struct ContactObjectMetadata
		{
			ContactTypeMetadata base; /**< C-style base to allow list initialization without constructor */
			unsigned interfaces;      /**< Interfaces supported by the object */
			const char *uri;          /**< Contacts API view URI which represent the object */
			unsigned idPropId;        /**< ID of property containing record ID */
			Utils::Range<const ContactFieldMetadata *> fields; /**< Fields that belong to the object */
		};

		/**
		 * @brief Typed object metadata.
		 */
		struct ContactTypedObjectMetadata
		{
			ContactObjectMetadata base;
			ContactFieldMetadata typeField;  /**< Field that stores object's "type" */
			ContactFieldMetadata labelField; /**< Field that stores object's custom type label */
		};

		/**
		 * @brief Array of objects metadata (object type specified by ContactObjectType in subType).
		 */
		struct ContactArrayMetadata
		{
			ContactTypeMetadata base;
			ContactFieldMetadata element; /**< Array element metadata */
		};

		/**
		 * @brief Enum type metadata.
		 *
		 * @see ContactFieldMetadata
		 */
		struct ContactEnumMetadata
		{
			ContactTypeMetadata base;
			Utils::Range<const int *> values; /**< Possible values */
			int defaultValue;    /**< Default value */
			int customValue;     /**< Value that signifies the custom value being set */
		};

		/**
		 * @param[in]   type    Contact object type (ObjectTypeContact or ObjectTypeMyProfile)
		 * @return Contact root object metadata.
		 */
		const ContactFieldMetadata *getContactMetadata(ContactObjectType type);
	}
}

#endif /* CONTACTS_MODEL_CONTACT_METADATA_H */
