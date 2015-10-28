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


#ifndef _CT_DETAIL_VIEW_BASIC_INFO_CONTROL_H_
#define _CT_DETAIL_VIEW_BASIC_INFO_CONTROL_H_

#include "WControl.h"

class CtDetailDataBase;

/**
 * @brief Represents base contact info
 */
class CtDetailViewBasicInfoControl : public WControl
{
public:
	/**
	 * @brief Create Basic info view
	 * @param[in]	data					Base detail data
	 * @param[in]	showFavoriteIcon		True if favorite icon should be shown
	 */
	CtDetailViewBasicInfoControl(CtDetailDataBase *data, bool showFavoriteIcon);

protected:
	virtual Evas_Object *onCreate(Evas_Object *parent, void *param);

private:
	int getLinesCount();
	void updateBaseLayout(Evas_Object *parent);
	void setThumbnail();
	void setFavoriteBtn();
	void setText(const char *text, const char *textPart);

	static void onBasicInfoFavoritesClickedCb(void *data, Evas_Object *obj, void *event_info);

private:
	CtDetailDataBase *m_DetailData;
	Evas_Object *m_BaseLayout;
	bool m_ShowFavoriteIcon;
	bool m_FavoriteReference;
};

#endif /* _CT_DETAIL_VIEW_BASIC_INFO_CONTROL_H_ */
