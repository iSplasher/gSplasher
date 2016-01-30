#include "../include/Widget.h"

USING_NAMESPACE

gCoreWidget::gCoreWidget(gCoreWidget* parent) :
	gCore(parent)
	{//style(0, 0, 200, 200) {
	is_widget = true;
	parent_widget = parent;
}

void gCoreWidget::update() {
}

void gCoreWidget::event(EventPtr ev) {
	// TODO: send events to children here
	switch (ev->type()) {
	case gEvent::MouseMove:
		mouseMoveEvent(std::static_pointer_cast<gMouseEvent>(ev));
		break;
	case gEvent::MouseButtonPress:
		mousePressEvent(std::static_pointer_cast<gMouseEvent>(ev));
		break;
	case gEvent::MouseButtonRelease:
		mouseReleaseEvent(std::static_pointer_cast<gMouseEvent>(ev));
		break;
	}
}

//Point gCoreWidget::pos() {
//	return Point(0, 0);
//}
//
//void gCoreWidget::move(Point new_p) {
//}
//
//void gCoreWidget::resize(Size new_s) {
//	style.size = new_s;
//}

//Point gCoreWidget::mapToGlobal(Point p) {
//	auto c_pos = pos();
//	Point n_pos(c_pos.x + p.x, c_pos.y + p.y);
//	return n_pos;
//}

void gCoreWidget::mousePressEvent(MouseEventPtr ev){
	printf("A button was pressed!");
	move_state = MoveState::Moving;
	//move_offset = ev->pos();
	//resize(style.size.width+5, style.size.height+5);
}

void gCoreWidget::mouseMoveEvent(MouseEventPtr ev) {
	//printf("x=%d y=%d\n", ev->global_x, ev->global_y);
	//if (ev->x >= 0 && ev->x < width() && ev->y >= 0 && ev->y < height()) {
	//	under_mouse = true;
	//} else {
	//	under_mouse = false;
	//}

	//if (move_state == MoveState::Moving) {
	//	move(mapToGlobal(ev->pos()) - move_offset);
	//}
}

void gCoreWidget::mouseReleaseEvent(MouseEventPtr ev) {
	printf("A button was released!");
	move_state = MoveState::Normal;
}

