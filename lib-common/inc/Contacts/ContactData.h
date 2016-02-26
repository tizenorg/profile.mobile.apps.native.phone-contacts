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

#ifndef CONTACTS_CONTACT_DATA_H
#define CONTACTS_CONTACT_DATA_H

#include <memory>
#include <tizen.h>
#include <vector>

namespace Contacts
{
	class ContactData;

	typedef std::unique_ptr<ContactData> ContactDataPtr;
	typedef std::vector<ContactData *> ContactDataList;

	class EXPORT_API ContactData
	{
	public:
		/**
		 * @brief Determine object type
		 */
		enum Type
		{
			TypeSpeeddial,  /**< Speeddial number */
			TypeLog,        /**< Log number */
			TypePerson      /**< Person */
		};

		/**
		 * @brief Represents ContactData changes on update event
		 */
		enum ChangedInfo
		{
			ChangedNone     = 0,      /**< Nothing was changed */
			ChangedName     = 1 << 0, /**< Name has changed */
			ChangedNumber   = 1 << 1, /**< Number has changed */
			ChangedImage    = 1 << 2  /**< Image path has changed */
		};

		/**
		 * @brief Change callback
		 * @param[in] Changed info
		 */
		typedef std::function<void(int)> UpdateCallback;

		/**
		 * @brief Delete callback
		 */
		typedef std::function<void()> DeleteCallback;

		/**
		 * @brief Create ContactData object
		 * @param[in]   type    ContactData type
		 */
		ContactData(Type type);
		virtual ~ContactData() { }

		/**
		 * @return ID
		 */
		virtual int getId() const = 0;

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

		/**
		 * @brief Determine if object searchable string contains @a str
		 * @return true if it contains, false if not
		 */
		virtual bool compare(const char *str) = 0;

		/**
		 * @brief Set update callback
		 * @param[in]   callback    Update callback
		 */
		void setUpdateCallback(UpdateCallback callback);

		/**
		 * @brief Unset update callback
		 */
		void unsetUpdateCallback();

		/**
		 * @brief Set delete callback
		 * @param[in]   callback    Delete callback
		 */
		void setDeleteCallback(DeleteCallback callback);

		/**
		 * @brief Unset delete callback
		 */
		void unsetDeleteCallback();

	protected:
		/**
		 * @brief ContactData updated callback
		 * @param[in]   changes     Changed info
		 */
		void onUpdated(int changes);

		/**
		 * @brief ContactData deleted callback
		 */
		void onDeleted();

	private:
		Type m_Type;
		UpdateCallback m_OnUpdated;
		DeleteCallback m_OnDeleted;
	};
}

#endif /* CONTACTS_CONTACT_DATA_H */
