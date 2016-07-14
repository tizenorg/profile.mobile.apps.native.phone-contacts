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

#ifndef CONTACTS_MODEL_SEARCH_RESULT_H
#define CONTACTS_MODEL_SEARCH_RESULT_H

#include "Common/Utils.h"
#include <memory>

namespace Contacts
{
	namespace Model
	{
		class SearchResult;

		typedef std::unique_ptr<SearchResult> SearchResultPtr;

		class EXPORT_API SearchResult
		{
		public:
			/**
			 * @brief Determines which field conform to searchable string
			 * @see compare()
			 */
			enum MatchedField
			{
				MatchedNone,         /**< Not matched */
				MatchedName,         /**< Matched by name */
				MatchedNickname,     /**< Matched by nickname */
				MatchedOrganization, /**< Matched by organization */
				MatchedAddress,      /**< Matched by contact address */
				MatchedEmail,        /**< Matched by contact email */
				MatchedNote,         /**< Matched by note field */
				MatchedNumber        /**< Matched by number */
			};

			SearchResult();

			/**
			 * @brief Create SearchResult object
			 * @param[in]   field   @see MatchedField
			 * @param[in]   str     Matched string
			 * @param[in]   substr  Matched substring
			 */
			SearchResult(MatchedField field, const char *str, ::Common::Substring substr);

			/**
			 * @return Matched field
			 * @see MatchedField
			 */
			MatchedField getMatchedField() const;

			/**
			 * @return Matched string
			 */
			const char *getMatchedString() const;

			/**
			 * @return Matched substring
			 */
			const ::Common::Substring &getMatchedSubstring() const;

		private:
			MatchedField m_MatchedField;
			const char *m_MatchedString;
			::Common::Substring m_MatchedSubstring;
		};
	}
}

#endif /* CONTACTS_MODEL_SEARCH_RESULT_H */
