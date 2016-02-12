﻿#pragma once

#include "gSplasher/Widget.h"

NAMESPACE_BEGIN

class gPainter;

class GSPLASHER_API gPen {
public:
	enum class Cap;
	enum class Join;

	gPen() = default;
	explicit gPen(gPainter &painter);

	/// <summary>
	/// Set a new color to this pen
	/// </summary>
	void setColor(gColor color);

	gColor color() const { return c_color; }

	/// <summary>
	/// Set a new line width to this pen
	/// </summary>
	void setWidth(float width);

	/// <summary>
	/// Get current width
	/// </summary>
	/// <returns>Current width</returns>
	float width() const { return c_width; }

	/// <summary>
	/// Set how the end of the line is drawn
	/// </summary>
	void setCap(Cap cap);

	/// <summary>
	/// Set how the corners are drawn
	/// </summary>
	void setJoin(Join join);

private:
	/// <summary>
	/// Applies the current pen style
	/// </summary>
	void apply() const;

	PainterContext* pc = nullptr;
	float c_width = 1;
	gColor c_color;

	friend class gPainter;
};

class GSPLASHER_API gBrush {
public:
	gBrush() = default;
	explicit gBrush(gPainter &painter);

	/// <summary>
	/// Set a new color to this brush
	/// </summary>
	void setColor(gColor color);

	gColor color() const { return c_color; }

	//void setGradient();
	//void setPattern();

private:
	/// <summary>
	/// Applies the current brush style
	/// </summary>
	void apply() const;

	PainterContext* pc = nullptr;
	gColor c_color;

	friend class gPainter;
};

class gTopBar;

class GSPLASHER_API gPainter {
public:
	gPainter(gWindow*);
	~gPainter();

	/// <summary>
	/// Begin painting
	/// </summary>
	void begin(float pixel_ratio=1);

	/// <summary>
	/// End painting
	/// </summary>
	void end();

	/// <summary>
	/// Set new pen
	/// </summary>
	/// <param name="pen">New pen</param>
	void setPen(gPen &pen);

	/// <summary>
	/// Retrieve current pen
	/// </summary>
	/// <returns>Current gPen</returns>
	gPen &pen() const { return *p; }

	/// <summary>
	/// Saves the current state
	/// </summary>
	/// <remarks>A matching restore() call must be used</remarks>
	void save();

	/// <summary>
	/// Restores saved state
	/// </summary>
	/// <remarks>A save() must have been called before calling this method</remarks>
	void restore();

	/// <summary>
	/// Resets to default state
	/// </summary>
	/// <remarks>This does not affect the state stack</remarks>
	void reset();

	/// <summary>
	/// Set new brush
	/// </summary>
	/// <param name="brush">New brush</param>
	void setBrush(gBrush &brush);

	/// <summary>
	/// Retrieve current brush
	/// </summary>
	/// <returns>Current gBrush</returns>
	gBrush &brush() const { return *b; }

	/// <summary>
	/// Draw a rectangle shape
	/// </summary>
	/// <param name="rect">Shape dimensions</param>
	void drawRect(gRectF rect) const;
	void drawRect(gRect rect) const { drawRect(gRectF(rect)); }

	/// <summary>
	/// Draw a rounded rectangle shape
	/// </summary>
	/// <param name="rect">Shape dimensions</param>
	/// <param name="radius">Radius of the rect corners</param>
	void drawRoundedRect(gRectF rect, float radius) const;
	void drawRoundedRect(gRect rect, int radius) const { drawRoundedRect(gRectF(rect), radius); }

	/// <summary>
	/// Draw an ellipse shape
	/// </summary>
	/// <param name="center">Center of ellipse</param>
	/// <param name="size">Width and height of ellipse</param>
	void drawEllipse(gPointF center, gSizeF size) const;

	/// <summary>
	/// Draw a circle shape
	/// </summary>
	/// <param name="center">Center of circle</param>
	/// <param name="radius">Circle radius</param>
	void drawCircle(gPointF center, float radius) const;

	/// <summary>
	/// Draw a line
	/// </summary>
	/// <param name="start">Start point of line</param>
	/// <param name="end">End point of line</param>
	void drawLine(gPointF start, gPointF end) const;

private:

	// helper methods
	void translate(gRectF &r) const;
	void translate(gPointF &p) const;
	void beginPath() const;
	void applyPB() const;

	gCoreWidget *w = nullptr;
	// Coordinates will be translated to this widget's parent
	gCoreWidget *origin_widget = nullptr;
	gCoreWidget *o_origin_widget = nullptr;
	PainterContext* context = nullptr;
	gPen *p = nullptr;
	gBrush *b = nullptr;
	bool begun = false;
	// TODO: maybe extend this to a gMargins? and do all sides?
	int top_margin = 0; // for window TopBar.. 

	friend class gCoreWidget;
	friend class gTopBar;
};

using UniquePainter = std::unique_ptr<gPainter>;
using SharedPainter = std::shared_ptr<gPainter>;

NAMESPACE_END