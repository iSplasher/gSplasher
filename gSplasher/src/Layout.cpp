﻿#include "gSplasher/Layout.h"
#include "gSplasher/Widget.h"

#include "LayoutImpl.h"


USING_NAMESPACE

gLayoutable::gLayoutable(gLayoutable* parent) : gCore(parent) {
	c_data = std::make_shared<priv::ItemConstraints>();
	setObjectName("gLayoutable");
}

gLayoutable::~gLayoutable() {}

void gLayoutable::setFixedWidth(int width) {
	c_data->fixed_w_constraint = true;
	c_data->fixed_width = width;
	auto l = layout();
	if (l) {
		l->invalidate();
	}
	c_data->width.change_value(width);
}

void gLayoutable::setFixedHeight(int height) {
	c_data->fixed_h_constraint = true;
	c_data->fixed_height = height;
	auto l = layout();
	if (l) {
		l->invalidate();
	}
	c_data->height.change_value(height);
}

gCoreWidget* gLayoutable::parentWidget() const {
	auto core_p = parentCore();
	return core_p ? core_p->is_widget || core_p->is_window ? static_cast<gCoreWidget*>(core_p) : nullptr : nullptr;
}

void gLayoutable::event(EventPtr ev) {
	switch (ev->type()) {
	case gEvent::Type::Resize:
		resizeEvent(std::static_pointer_cast<gResizeEvent>(ev));
		break;
	}

	gCore::event(ev);
}

gPoint gLayoutable::pos() const {
	return gPoint(c_data->x.int_value(), c_data->y.int_value());
}

void gLayoutable::move(gPoint new_p) {
	if (!containing_layout) {
		c_data->x.change_value(new_p.x);
		c_data->y.change_value(new_p.y);
	}
}

void gLayoutable::resize(gSize new_s) {
	gApp->dispatchEvent(this, std::make_shared<gResizeEvent>(gEvent::Type::Resize, new_s, size()));
}

gSize gLayoutable::size() const {
	return gSize(c_data->width.int_value(), c_data->height.int_value());
}

void gLayoutable::resizeEvent(ResizeEventPtr ev) {
	if (!containing_layout) {
		c_data->fixed_w_constraint = false;
		c_data->fixed_h_constraint = false;
		c_data->width.change_value(ev->new_size.width);
		c_data->height.change_value(ev->new_size.height);
	}
}

gLayout::gLayout(gCoreWidget* parent) : gLayoutable(parent) {
	layouter = std::make_shared<priv::LayoutImpl>(this);
	if (parent) {
		setWigdet(parent);
	}
	setObjectName("gLayout");
}

void gLayout::setWigdet(gCoreWidget* new_parent) {
	if (!new_parent->layout()) {
		layouter->setWidget(new_parent);
		new_parent->_layout = this;
	}
	else {
		std::cout << "This wigdet is already handled by a layout\n";
	}
}

void gLayout::add(gLayoutable* item, Alignment align) {
	item->setParent(parentWidget());
	item->containing_layout = this;
	layouter->addItem(item);
	invalidate();
}

void gLayout::invalidate() {

}

void gLayout::event(EventPtr ev) {
	switch (ev->type()) {
	case gEvent::Type::Layout:
		std::cout << "invalidate event" << std::endl;
		invalidate();
		break;
	}
	gLayoutable::event(ev);
}

void gLayout::beginLayoutChange() const {
	
	for (auto &i : layouter->items()) {
		i->c_data->old_size = gSize(i->c_data->width.int_value(), i->c_data->height.int_value());
	}

}

void gLayout::endLayoutChange() const {

	for (auto &i : layouter->items()) {
		gApp->sendEvent(i, std::make_shared<gResizeEvent>(gEvent::Type::Resize,
			gSize(i->c_data->width.int_value(), i->c_data->height.int_value()), i->c_data->old_size));
	}
}
