#pragma once

#include "global.h"
#include "property.h"
#include "core.h"
#include "core/primitive.h"

#include <unordered_map>

NAMESPACE_BEGIN
class Widget;
class Layout;

using WidgetPtr = std::unique_ptr< Widget >;
using LayoutPtr = std::unique_ptr< Layout >;

PRIV_NAMESPACE_BEGIN
#include <yoga/Yoga.h>

class Layoutable;

using LayoutNode = YGNodeRef;
using LayoutablePtr = std::unique_ptr< Layoutable >;


class ELEMENT_API Layoutable : public Element {
public:

	// *structers
	explicit Layoutable( Layoutable* parent = nullptr );

	virtual ~Layoutable();

	// POSITION & SIZE 

	Property<PointF> position;
	Property<SizeF> size;
	Property<SizeF> minSize;
	Property<SizeF> maxSize;
	PropertyView< RectF > geometry;
	//const PropertyView< Rect > contentGeometry;

	// Margin

	Property<float> marginLeft;
	Property<float> marginTop;
	Property<float> marginRight;
	Property<float> marginBottom;
	//PropertyView< RectF > margin;

	// BORDER

	Property<float> borderLeft;
	Property<float> borderTop;
	Property<float> borderRight;
	Property<float> borderBottom;
	//PropertyView< RectF > border;

	// PADDING
	Property<float> paddingLeft;
	Property<float> paddingTop;
	Property<float> paddingRight;
	Property<float> paddingBottom;
	//PropertyView< RectF > padding;

	// STYLE

	Property<Alignment> alignment;
	Property<float> growth;
	Property<bool> absolutePosition;

	
	/**
	 * \brief Returns the layout this item is contained in
	 */
	const Accessor< Layout*, Layoutable > layout;


	virtual void update();

private:

	virtual void updateChildren();

	virtual void invalidated();

	Layout* getLayout() const;

	// data members

	bool dirty_layout = false; // layout has to recalculate

	Layout* playout = nullptr; // layout this item is contained in
	Layout* bound_layout = nullptr; // layout that manages items for this widget
	LayoutNode node = nullptr;

	friend class Layout;
};


NAMESPACE_END


/**
 * \brief Layout class. Derive this class to make a custom layout. 
 */
class ELEMENT_API Layout : public PRIV_NAMESPACE::Layoutable {
	friend class Widget;
public:
	// * structers
	explicit Layout();

	virtual ~Layout() = default;

	// POSITION & SIZE 

	PropertyView<PointF> position;
	PropertyView<SizeF> size;

	// STYLE

	Property<Orientation> orientation;
	Property<bool> wrap;

	const Accessor< Widget* , Layout > widget;


	// member methods

	virtual void append( PRIV_NAMESPACE::Layoutable* item, Alignment align = Alignment::Default, float grow = 1 );
	virtual void append( std::initializer_list<PRIV_NAMESPACE::Layoutable*> item, Alignment align = Alignment::Default, float grow = 1 );

	void update() override;

	/// <summary>
	/// Take item out of layout. 
	/// </summary>
	/// <param name="item">item to take out</param>
	void take( PRIV_NAMESPACE::Layoutable* item );

	//void remove(LayoutCore&);

	//virtual int count() const;
	//bool isEmpty() const;

	virtual void invalidate();

protected:

	//void beginLayoutChange() const;

	//void endLayoutChange() const;

private:

	// member methods

	void invalidated() override;

	static void applySet( PRIV_NAMESPACE::Layoutable* item, Alignment align, float grow );
	static void applyGet( PRIV_NAMESPACE::Layoutable* item);

	// data
	std::unordered_map< priv::LayoutNode, PRIV_NAMESPACE::Layoutable* > nodemap;
	Widget* _widget = nullptr;

};


NAMESPACE_END
