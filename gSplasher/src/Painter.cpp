﻿#include "gSplasher/Utils/Painter.h"
#include "gSplasher/Window.h"
#include "gSplasher/Utils/Primitives.h"

#include <GL/glew.h> // required for nanovg
#include <GLFW/glfw3.h> // required for nanovg

#include "nanovg/nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/nanovg_gl.h"

USING_NAMESPACE

_PColor gColor::toPColor() const {
	NVGcolor c;
	switch (type) {
	case RGB:
		c = nvgRGB(red, green, blue);
		break;
	case RGBA:
		c = nvgRGBA(red, green, blue, alpha);
		break;
	case HSL:
		c = nvgHSL(hue, sat, light);
		break;
	case HSLA:
		c = nvgHSLA(hue, sat, light, alpha);
	}

	return c;
}

enum class gPen::Cap {
	Butt = NVG_BUTT,
	Round = NVG_ROUND,
	Square = NVG_SQUARE
};

enum class gPen::Join {
	Miter = NVG_MITER,
	Round = NVG_ROUND,
	Bevel = NVG_BEVEL
};

gPen::gPen(gPainter& painter) {
	painter.setPen(*this);
	setJoin(Join::Bevel);
	setCap(Cap::Round);
	setColor(gColor(0, 0, 0));
	setWidth(1);
}

void gPen::setColor(gColor color) {
	if (pc) {
		nvgStrokeColor(pc, color.toPColor());
		c_color = color;
	}
}

void gPen::setWidth(float width) {
	if (pc) {
		nvgStrokeWidth(pc, width);
		c_width = width;
	}
}

void gPen::setCap(Cap cap) {
	if (pc) {
		nvgLineCap(pc, static_cast<int>(cap));
	}
}

void gPen::setJoin(Join join) {
	if (pc) {
		nvgLineJoin(pc, static_cast<int>(join));
	}
}

void gPen::apply() const {
	if (pc) {
		nvgStroke(pc);
	}
}

gBrush::gBrush(gPainter& painter) {
	painter.setBrush(*this);
	setColor(gColor(0, 0, 0));
}

void gBrush::setColor(gColor color) {
	if (pc) {
		nvgFillColor(pc, color.toPColor());
		c_color = color;
	}
}

void gBrush::apply() const {
	if (pc) {
		nvgFill(pc);
	}
}

gsp::gPainter::gPainter(gWindow* window) {
	w = window;
	if (!w->this_paint) {
		w->this_paint = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
		if (!w->this_paint) {
			std::cout << "Fatal: Could not create paint context\n";
		}
	}
	context = w->this_paint;
}

gPainter::~gPainter() {
}

void gPainter::begin(float pixel_ratio) {
	if (begun) {
		throw std::runtime_error("Inconsistent gPainter::begin call");
	}
	begun = true;
	w->parent_window->setActive();
	if (w->parent_window->top_bar) {
		top_margin = w->parent_window->top_bar->size().height;
	}
	auto s = w->size();
	nvgBeginFrame(context, s.width, s.height, pixel_ratio);
}

void gPainter::end() {
	if (!begun) {
		throw std::runtime_error("Inconsistent gPainter::end call");
	}
	begun = false;
	nvgEndFrame(context);
}

void gPainter::setPen(gPen& pen) {
	pen.pc = context;
	p = &pen;
}

void gPainter::save() {
	nvgSave(context);
	o_origin_widget = origin_widget;
}

void gPainter::restore() {
	nvgRestore(context);
	origin_widget = o_origin_widget;
}

void gPainter::reset() {
	nvgReset(context);
	o_origin_widget = nullptr;
	origin_widget = nullptr;
}

void gPainter::setBrush(gBrush& brush) {
	brush.pc = context;
	b = &brush;
}

void gPainter::drawRect(gRectF rect) const {
	beginPath();
	translate(rect);
	nvgRect(context, rect.x, rect.y, rect.width, rect.height);
	applyPB();
}

void gPainter::drawRoundedRect(gRectF rect, float radius) const {
	beginPath();
	translate(rect);
	nvgRoundedRect(context, rect.x, rect.y, rect.width, rect.height, radius);
	applyPB();
}

void gPainter::drawEllipse(gPointF center, gSizeF size) const {
	beginPath();
	translate(center);
	nvgEllipse(context, center.x, center.y, size.width, size.height);
	applyPB();
}

void gPainter::drawCircle(gPointF center, float radius) const {
	beginPath();
	translate(center);
	nvgCircle(context, center.x, center.y, radius);
	applyPB();
}

void gPainter::drawLine(gPointF start, gPointF end) const {
	beginPath();
	translate(start);
	translate(end);
	nvgMoveTo(context, start.x, start.y);
	nvgLineTo(context, end.x, end.y);
	applyPB();
}

void gPainter::translate(gRectF& r) const {
	// TODO: fix this conversion from float to int
	if (origin_widget) {
		auto p = origin_widget->mapToParent(gPoint(r.x, r.y));
		r = p;
	}
	r.y += top_margin;
}

void gPainter::translate(gPointF& p) const {
	// TODO: fix this conversion from float to int
	if (origin_widget) {
		auto p2 = origin_widget->mapToParent(gPoint(p));
		p.x = p2.x;
		p.y = p2.y;
	}

	p.y += top_margin;
}

void gPainter::beginPath() const {
	nvgBeginPath(context);
}

void gPainter::applyPB() const {
	if (p) {
		p->apply();
	}
	if (b) {
		b->apply();
	}
}