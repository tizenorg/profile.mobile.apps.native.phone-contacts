/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef _PH_DIALER_SEARCH_INFO_H_
#define _PH_DIALER_SEARCH_INFO_H_

#include <string>
#include <contacts.h>

/**
 * @brief This class provides contact info for predictive number functionality
 */
namespace Ph {
	namespace Dialer {

		/**
		 * @brief Specifies the source of predictive search result item.
		 */
		enum InfoType {
			IT_NONE = -1,   /**< Invalid result */
			IT_SPEED_DIAL,  /**< From speed dial */
			IT_LOG,         /**< From call logs */
			IT_NAME,        /**< From contacts matched by name */
			IT_NUMBER,      /**< From contacts matched by number */
			IT_MAX          /**< Sentinel value */
		};

		/**
		 * @brief Predictive search result item.
		 */
		class SearchInfo
		{
		public:
			SearchInfo(InfoType type, const contacts_record_h record = NULL);

			/**
			 * @brief Gets searchable string, according to info type
			 * @return Returns searchable string
			 */
			std::string getSearchString() const;

			/**
			 * @return info type value on successfully constructed object, otherwise IT_NONE value
			 */
			InfoType getType() const;

			/**
			 * @return contact ID on success or 0 if object constructed with errors
			 */
			int getId() const;

			/**
			 * @return speeddial ID on success or 0 if object constructed with errors
			 */
			int getSpeedDialId() const;

			/**
			 * @brief Gets normal or highlighted name
			 * @param[in]	isHighlighted		Should highlighted name be returned
			 * @return name of contact
			 *
			 * @remark This function returns reference to string,
			 * 			it should be used before object destruction
			 */
			const std::string &getName(bool isHighlighted) const;

			/**
			 * @brief Gets normal or highlighted number
			 * @param[in]	isHighlighted		Should highlighted number be returned
			 * @return number of contact
			 *
			 * @remark This function returns reference to string,
			 * 			it should be used before object destruction
			 */
			const std::string &getNumber(bool isHighlighted) const;

			/**
			 * @return highlighted text
			 *
			 * @remark This function returns reference to string,
			 * 			it should be used before object destruction
			 */
			const std::string &getHighlightedText() const;

			/**
			 * @brief Highlights match text
			 * @param[in]	searchStr		Search string
			 * @return true if text was highlighted
			 */
			bool updateHighlightText(const std::string searchStr, size_t position);

		private:
			bool fillWithRecord(InfoType type, const contacts_record_h record);
			void fillSpeedDial(const contacts_record_h record);
			void fillLog(const contacts_record_h record);
			void fillContact(InfoType type, const contacts_record_h record);

			InfoType m_Type;
			int m_Id;
			int m_SpeedDialId;
			std::string m_MaskedName;
			std::string m_Name;
			std::string m_Number;
			std::string m_HighlightedText;
		};
	}
}

#endif //_PH_DIALER_SEARCH_INFO_H_
