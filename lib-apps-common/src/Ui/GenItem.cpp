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

#include "Ui/GenItem.h"
#include "Ui/GenContainer.h"
#include "Ui/GenGroupItem.h"
#include "Utils/Callback.h"

using namespace Ui;

typedef void(*ScrollFunc)(Elm_Object_Item *item, int type);

struct Ui::GenItemApi
{
	Elm_Object_Item *(*getParent)(const Elm_Object_Item *item);
	Elm_Object_Item *(*getNext)(const Elm_Object_Item *item);
	Elm_Object_Item *(*getPrev)(const Elm_Object_Item *item);
	ScrollFunc bringIn;
	ScrollFunc show;
	void(*setSelected)(Elm_Object_Item *item, Eina_Bool selected);
};

namespace
{
	const GenItemApi api[] = {
		{ elm_genlist_item_parent_get,
		  elm_genlist_item_next_get,
		  elm_genlist_item_prev_get,
		  (ScrollFunc) elm_genlist_item_bring_in,
		  (ScrollFunc) elm_genlist_item_show,
		  elm_genlist_item_selected_set },

		{ [](const Elm_Object_Item *item) -> Elm_Object_Item * { return nullptr; },
		  elm_gengrid_item_next_get,
		  elm_gengrid_item_prev_get,
		  (ScrollFunc) elm_gengrid_item_bring_in,
		  (ScrollFunc) elm_gengrid_item_show,
		  elm_gengrid_item_selected_set }
	};
}

GenItem::GenItem(GenContainer::Type type)
	: m_Item(nullptr), m_Preserve(false), m_IsRealized(false), m_IsFocusPending(false),
	  m_Api(&api[type])
{
}

GenItem::~GenItem()
{
	if (m_OnDestroy) {
		m_OnDestroy();
	}

	if (m_Item) {
		pop();
	}
}

bool GenItem::isRealized() const
{
	return m_IsRealized;
}

bool GenItem::isInserted() const
{
	return m_Item != nullptr;
}

Elm_Object_Item *GenItem::getObjectItem() const
{
	return m_Item;
}

GenGroupItem *GenItem::getParentItem() const
{
	Elm_Object_Item *objectItem = m_Api->getParent(getObjectItem());
	GenItem *item = (GenItem *) elm_object_item_data_get(objectItem);
	return dynamic_cast<GenGroupItem *>(item);
}

GenContainer *GenItem::getParent() const
{
	Evas_Object *genlist = elm_object_item_widget_get(getObjectItem());
	return static_cast<GenContainer *>(Control::getControl(genlist));
}

GenItem *GenItem::getNextItem() const
{
	Elm_Object_Item *item = m_Api->getNext(getObjectItem());
	return (GenItem *) elm_object_item_data_get(item);
}

GenItem *GenItem::getPrevItem() const
{
	Elm_Object_Item *item = m_Api->getPrev(getObjectItem());
	return (GenItem *) elm_object_item_data_get(item);
}

GenItemPtr GenItem::getWeakPtr()
{
	if (!m_SelfPtr) {
		m_SelfPtr.reset(this, [](GenItem *){ });
	}

	return m_SelfPtr;
}

void GenItem::setSelectCallback(SelectCallback callback)
{
	m_OnSelected = std::move(callback);
}

void GenItem::setDestroyCallback(DestroyCallback callback)
{
	m_OnDestroy = std::move(callback);
}

void GenItem::scrollTo(int position, bool isAnimated)
{
	auto scroll = isAnimated ? m_Api->bringIn : m_Api->show;
	scroll(getObjectItem(), position);
}

void GenItem::focus(Elm_Genlist_Item_Scrollto_Type position, bool isAnimated)
{
	scrollTo(position, isAnimated);
	if (isRealized()) {
		onFocused();
	} else {
		m_IsFocusPending = true;
	}
}

void GenItem::pop()
{
	onPop();

	m_Preserve = true;
	elm_object_item_del(m_Item);
	m_Preserve = false;
}

Elm_Gen_Item_Class GenItem::createItemClass(const char *style,
		const char *decorateStyle, const char *editStyle)
{
	Elm_Gen_Item_Class itc = { ELM_GEN_ITEM_CLASS_HEADER, 0 };
	itc.item_style = style;
	itc.decorate_item_style = decorateStyle;
	itc.decorate_all_item_style = editStyle;
	itc.func.text_get = makeCallback(&GenItem::getText);
	itc.func.content_get = makeCallback(&GenItem::getContent);
	itc.func.state_get = makeCallback(&GenItem::getState);
	itc.func.filter_get = makeCallback(&GenItem::compare);
	itc.func.del = makeCallback(&GenItem::onDestroy);

	return itc;
}

Elm_Gen_Item_Class *GenItem::getItemClass() const
{
	static Elm_Gen_Item_Class itc = createItemClass("type1");
	return &itc;
}

void GenItem::onInserted(Elm_Object_Item *item)
{
	m_Item = item;
	onInserted();
}

void GenItem::onDestroy(Evas_Object *genlist)
{
	m_Item = nullptr;
	if (!m_Preserve) {
		delete this;
	}
}

void GenItem::onSelected(Elm_Object_Item *item)
{
	m_Api->setSelected(item, EINA_FALSE);
	onSelected();

	if (m_OnSelected) {
		m_OnSelected();
	}
}

void GenItem::onRealized(Elm_Object_Item *item)
{
	if (!m_Item) {
		m_Item = item;
	}

	m_IsRealized = true;
	if (m_IsFocusPending) {
		onFocused();
		m_IsFocusPending = false;
	}

	onRealized();
}

void GenItem::onUnrealized(Elm_Object_Item *item)
{
	m_IsRealized = false;
	onUnrealized();
}
