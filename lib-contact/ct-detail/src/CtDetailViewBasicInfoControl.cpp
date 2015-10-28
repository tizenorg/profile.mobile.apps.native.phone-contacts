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


#include "ContactsThumbnail.h"
#include "CtCommon.h"
#include "CtDetailDataBase.h"
#include "CtDetailViewBasicInfoControl.h"
#include "CtPath.h"
#include "CtRegistryMgr.h"
#include "WDebugBase.h"



CtDetailViewBasicInfoControl::CtDetailViewBasicInfoControl(CtDetailDataBase *data, bool showFavoriteIcon)
{
	m_DetailData = data;
	m_BaseLayout = NULL;
	m_ShowFavoriteIcon = showFavoriteIcon;
	m_FavoriteReference = false;
}

Evas_Object *CtDetailViewBasicInfoControl::onCreate(Evas_Object *parent, void *viewParam)
{
	WHIT();
	WPRET_VM(!m_DetailData, NULL, "__detailData is NULL");

	m_BaseLayout = elm_layout_add(parent);
	updateBaseLayout(parent);

	setThumbnail();
	setFavoriteBtn();
	setText(m_DetailData->getDisplayName(), "text_1line");

	if ((m_DetailData->getPhoneticName() && *m_DetailData->getPhoneticName())) {
		setText(m_DetailData->getPhoneticName(), "text_2line");

		if ((m_DetailData->getCompany() && *m_DetailData->getCompany())) {
			setText(m_DetailData->getCompany(), "text_3line");
		}

	} else if ((m_DetailData->getCompany() && *m_DetailData->getCompany())) {
		setText(m_DetailData->getCompany(), "text_2line");
	}

	return m_BaseLayout;
}

int CtDetailViewBasicInfoControl::getLinesCount()
{
	WPRET_VM(!m_DetailData, -1, "__detailData is NULL");
	int count = 0;

	if (m_DetailData->getPhoneticName() && *m_DetailData->getPhoneticName()) {
		++count;
	}
	if (m_DetailData->getCompany() && *m_DetailData->getCompany()) {
		++count;
	}
	WDEBUG("count %d", count);

	return count;
}

void CtDetailViewBasicInfoControl::updateBaseLayout(Evas_Object *parent)
{
	WPRET_M(!m_DetailData, "__detailData is NULL");

	const char *signal[] = {
		"set_1line",
		"set_2lines",
		"set_3lines"
	};
	const char *groupName[] = {
		"base_info_1line",
		"base_info_2lines",
		"base_info_3lines"
	};
	int count = getLinesCount();
	if (count >= 0) {
		WDEBUG("signal: %s, group name: %s", signal[count], groupName[count]);

		elm_object_signal_emit(parent, signal[count], "");
		elm_layout_file_set(m_BaseLayout, ContactsCommon::getResourcePath(DETAIL_LAYOUT_EDJ).c_str(), groupName[count]);
	}
}

void CtDetailViewBasicInfoControl::setThumbnail()
{
	WPRET_M(!m_DetailData, "__detailData is NULL");

	Evas_Object *thumbnail = createThumbnail(m_BaseLayout, THUMBNAIL_240, m_DetailData->getImagePath());
	elm_object_part_content_set(m_BaseLayout, "swallow.image_thumbnail", thumbnail);
}

void CtDetailViewBasicInfoControl::setFavoriteBtn()
{
	WPRET_M(!m_DetailData, "__detailData is NULL");

	if (m_ShowFavoriteIcon) {
		m_FavoriteReference = m_DetailData->getFavorite();
		Evas_Object *check = elm_check_add(m_BaseLayout);
		elm_object_style_set(check, "favorite");
		elm_check_state_set(check, m_FavoriteReference);
		evas_object_smart_callback_add(check, "changed", onBasicInfoFavoritesClickedCb, this);
		elm_layout_content_set(m_BaseLayout, "swallow.favor_btn", check);
	}
}

void CtDetailViewBasicInfoControl::setText(const char *text, const char *textPart)
{
	char *markupText = elm_entry_utf8_to_markup(text);
	if (markupText && *markupText) {
		WDEBUG("text = %s", markupText);
		elm_object_part_text_set(m_BaseLayout, textPart, markupText);
	}
	free(markupText);
}

void CtDetailViewBasicInfoControl::onBasicInfoFavoritesClickedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	WPRET_M(!data, "data is NULL");

	CtDetailViewBasicInfoControl *basicInfo = (CtDetailViewBasicInfoControl*) data;
	WPRET_M(!basicInfo->m_DetailData, "__detailData is NULL");

	CtRegistryMgr::getInstance()->setInt(REGISTRY_KEY_FAVORITE_UPDATED, 1);

	basicInfo->m_FavoriteReference = !basicInfo->m_FavoriteReference;
	basicInfo->m_DetailData->setFavorite(basicInfo->m_FavoriteReference);

	WDEBUG("favorite %d", basicInfo->m_DetailData->getFavorite());
}
