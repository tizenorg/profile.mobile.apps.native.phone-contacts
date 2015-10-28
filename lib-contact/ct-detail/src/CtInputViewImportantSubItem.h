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

#ifndef _CT_INPUT_VIEW_IMPORTANT_SUB_ITEM_H_
#define _CT_INPUT_VIEW_IMPORTANT_SUB_ITEM_H_

#include "CtInputViewGenlistItem.h"
#include "CtDetailDataBase.h"
#include "CtInputViewControl.h"
#include "CtData.h"

class CtInputViewImportantSubItem : public CtInputViewGenlistItem
{
public:
	CtInputViewImportantSubItem(int index, bool isMyProfile): __index(index), __isMyProfile(isMyProfile)
	{
	};
	virtual ~CtInputViewImportantSubItem() { WHIT(); }
public:
	virtual Elm_Genlist_Item_Class* getItemClassStatic() {
		static Elm_Genlist_Item_Class itc = {};
		if( itc.item_style )
			return &itc;

		itc.item_style = "type1";
		itc.func.text_get = __getTextImportantSubitem;
		itc.func.del = WGENLIST_ITEM_DEL();
		return &itc;
	}

	virtual bool getIsEmpty()
	{
		return true;
	}

	int getIndex()
	{
		return __index;
	}

private:
	static char* __getTextImportantSubitem(void *data, Evas_Object *obj, const char *part)
	{
		//WHIT();
		CtInputViewImportantSubItem* item = (CtInputViewImportantSubItem*)data;
		if (0 == strcmp(part, "elm.text")) {
			if (item->__isMyProfile) {
				switch (item->__index) {
				case MYPROFILE_INPUT_IMPORTANT_PHONETIC_INFO:
					return strdup(V_("IDS_PB_BODY_PHONETIC_NAME_ABB"));
				case MYPROFILE_INPUT_IMPORTANT_MESSENGER:
					return strdup(V_("IDS_PB_BODY_IM"));
				case MYPROFILE_INPUT_IMPORTANT_URL:
					return strdup(V_("IDS_PB_BODY_WEBSITE_ABB"));
				case MYPROFILE_INPUT_IMPORTANT_ORGANIZATION:
					return strdup(V_("IDS_PB_OPT_ORGANISATION_ABB"));
				case MYPROFILE_INPUT_IMPORTANT_NICKNAME:
					return strdup(V_("IDS_PB_BODY_NICKNAME_ABB"));
				case MYPROFILE_INPUT_IMPORTANT_EVENT:
					return strdup(V_("IDS_PB_MBODY_EVENT"));
				case MYPROFILE_INPUT_IMPORTANT_ADDRESS:
					return strdup(V_("IDS_PB_BODY_ADDRESS_ABB2"));
				default:
					return NULL;
				}
			}
			else {
				switch (item->__index) {
				case INPUT_IMPORTANT_PHONETIC_INFO:
					return strdup(V_("IDS_PB_BODY_PHONETIC_NAME_ABB"));
				case INPUT_IMPORTANT_MESSENGER:
					return strdup(V_("IDS_PB_BODY_IM"));
				case INPUT_IMPORTANT_URL:
					return strdup(V_("IDS_PB_BODY_WEBSITE_ABB"));
				case INPUT_IMPORTANT_ORGANIZATION:
					return strdup(V_("IDS_PB_OPT_ORGANISATION_ABB"));
				case INPUT_IMPORTANT_RELATIONSHIP:
					return strdup(V_("IDS_PB_OPT_RELATIONSHIP_ABB2"));
				case INPUT_IMPORTANT_NICKNAME:
					return strdup(V_("IDS_PB_BODY_NICKNAME_ABB"));
				case INPUT_IMPORTANT_EVENT:
					return strdup(V_("IDS_PB_MBODY_EVENT"));
				case INPUT_IMPORTANT_ADDRESS:
					return strdup(V_("IDS_PB_BODY_ADDRESS_ABB2"));
				case INPUT_IMPORTANT_ADDRBOOK:
					return strdup(V_("IDS_PB_OPT_SAVE_CONTACT_TO"));
				default:
					return NULL;
				}
			}
		}

		return NULL;
	}

private:
	CtInputViewImportantSubItem();
	typedef enum {
		MYPROFILE_INPUT_IMPORTANT_ADDRESS = 1,
		MYPROFILE_INPUT_IMPORTANT_EVENT,
		MYPROFILE_INPUT_IMPORTANT_NICKNAME,
		MYPROFILE_INPUT_IMPORTANT_ORGANIZATION,
		MYPROFILE_INPUT_IMPORTANT_URL,
		MYPROFILE_INPUT_IMPORTANT_MESSENGER,
		MYPROFILE_INPUT_IMPORTANT_PHONETIC_INFO,
		MYPROFILE_INPUT_IMPORTANT_MAX,
	}CtMyProfileInputSubItems;

	typedef enum {
		INPUT_IMPORTANT_ADDRBOOK,
		INPUT_IMPORTANT_ADDRESS,
		INPUT_IMPORTANT_EVENT,
		INPUT_IMPORTANT_NICKNAME,
		INPUT_IMPORTANT_RELATIONSHIP,
		INPUT_IMPORTANT_ORGANIZATION,
		INPUT_IMPORTANT_URL,
		INPUT_IMPORTANT_MESSENGER,
		INPUT_IMPORTANT_PHONETIC_INFO,
		INPUT_IMPORTANT_MAX,

	}CtInputSubItems;

	int __index;
	bool __isMyProfile;
};

#endif /* _CT_INPUT_VIEW_IMPORTANT_SUB_ITEM_H_ */
