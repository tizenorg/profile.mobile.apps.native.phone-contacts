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

#ifndef CONTACTS_MODEL_CONTACT_DATA_H
#define CONTACTS_MODEL_CONTACT_DATA_H

#include <memory>
#include <tizen.h>
#include <list>
#include "Common/DataTypes.h"
#include "Model/DataItem.h"

namespace Contacts
{
	namespace Model
	{
		class EXPORT_API ContactData : public ::Model::DataItem
		{
		public:
			/**
			 * @brief Determine object type
			 */
			enum Type
			{
				TypeSpeeddial,    /**< Speeddial number */
				TypeLog,          /**< Log number */
				TypePerson,       /**< Person */
				TypeContact,      /**< Contact */
				TypeContactNumber /**< Contact number */
			};

			/**
			 * @brief Determine object fields
			 */
			enum Field
			{
				FieldFirst,
				FieldName = FieldFirst, /**< Name */
				FieldNumber,            /**< Number */
				FieldImage,             /**< Image path */
				FieldMax                /**< Sentinel value */
			};

			/**
			 * @brief Represents ContactData changes on update event
			 */
			enum ChangedInfo
			{
				ChangedNone     = 0,                /**< Nothing was changed */
				ChangedName     = 1 << FieldName,   /**< Name has changed */
				ChangedNumber   = 1 << FieldNumber, /**< Number has changed */
				ChangedImage    = 1 << FieldImage   /**< Image path has changed */
			};

			/**
			 * @brief Create ContactData object
			 * @param[in]   type    ContactData type
			 */
			ContactData(Type type);
			ContactData(const ContactData &that) = delete;
			virtual ~ContactData() { }

			ContactData &operator=(const ContactData &that) = delete;

			/**
			 * @return Name
			 */
			virtual const char *getName() const = 0;

			/**
			 * @return Number
			 */
			virtual const char *getNumber() const = 0;

			/**
			 * @return Image path
			 */
			virtual const char *getImagePath() const = 0;

			/**
			 * @return ContactData type
			 */
			Type getType() const;

		private:
			Type m_Type;
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_DATA_H */
