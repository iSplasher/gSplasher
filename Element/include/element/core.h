#pragma once

#include "global.h"
#include "event.h"
#include "property.h"
#include "core/_tree.h"

#include <atomic>
#include <thread>

NAMESPACE_BEGIN
class Element;
using ElementPtr = std::unique_ptr< Element >;
class Window;
class Application;


enum class ElementType {
	Base, // TODO: new name
	Application,
	Widget,
	Window,
};

enum class ExitCode {
	Quit,
	Restart
};


/**
 * \brief Base object.
 * A Element is created with Application.create<Element>(args...);
 */
class ELEMENT_API Element {
public:

	Element();

	explicit Element( Element *parent );

	virtual ~Element();

	// MOVE & COPY

	Element( const Element& other ) = delete;

	Element& operator=( const Element& other ) = delete;


	// PROPERTIES

	Property< std::string > objectName;
	Property< ElementType, Element > type{ ElementType::Base };

	const Accessor< Element* , Element > parent;

	bool blockEvents = false;


	// FUNCTIONS

	std::vector< Element* > children();

	// DATA

protected:

	void setType( ElementType t ) { type = t; }
	virtual void setParent(Element*);
	Element* getParent() const;


private:
	using ElementTree = PRIV_NAMESPACE::tree< Element* >;


	Element(Element&& other) noexcept
		: objectName(std::move(other.objectName)),
		type(std::move(other.type)),
		parent(std::move(other.parent)),
		core_id(other.core_id),
		internal_tree(std::move(other.internal_tree)),
		_parent(other._parent),
		init(other.init),
		parent_is_deleting(other.parent_is_deleting) {}

	Element& operator=(Element&& other) noexcept {
		if (this == &other)
			return *this;
		objectName = std::move(other.objectName);
		type = std::move(other.type);
		parent = std::move(other.parent);
		core_id = other.core_id;
		internal_tree = std::move(other.internal_tree);
		_parent = other._parent;
		init = other.init;
		parent_is_deleting = other.parent_is_deleting;
		return *this;
	}

	// FUNCTIONS

	//log(LogLevel, std::string);

	// DATA

	unsigned core_id;
	static std::atomic< unsigned > id_counter;
	ElementTree::iterator internal_tree;
	Element* _parent = nullptr;
	ElementPtr* object = nullptr;

	bool init = false; // has this Element initialized?

	/**
	 * \brief Used by _parent to let us know if we should remove ourselves from the internal tree container
	 */
	bool parent_is_deleting = false;

	friend class Application;
	friend class Window;
};


/**
 * \brief Main instance of the whole application. Only one instance is allowed.
 */
class ELEMENT_API Application final : private Element {
	using ElementContainer = std::list< ElementPtr >;
	using ElementContainerPtr = std::unique_ptr< ElementContainer >;
	using ElementTreePtr = std::unique_ptr< ElementTree >;

public:

	Application();

	~Application();

	// PROPERTIES

	const Accessor< std::size_t, Application > elementCount;
	/**
	 * \brief Get/Set doubleclick interval in milliseconds. Default value is 500.
	 */
	const Accessor< float, Application > doubleClickInterval;
	/**
	 * \brief Get/Set click interval in milliseconds. Default value is 350.
	 */
	const Accessor< float, Application > clickInterval;

	// FUNCTIONS

	/**
	 * \brief Create \ref Element or a derived objects
	 * \tparam T \ref Element or a derived object
	 * \tparam Args Parameter pack
	 * \param args Arguments that will be passed to the object's constructor. Think of it like `std::make_unique<T>(args...)`
	 * \return A reference to an unique pointer to the object
	 * \remark The application will own the object. To pass around the object, you must *pass by reference*.
	 * It is advised to know about `smart pointers`, specifically a `unique_pointer` and its semantics.
	 * \warning You must use this interface to create an \ref Element or a derived object
	 * \see destroy
	 */
	template< class T, typename... Args >
	T* create( Args&& ... args );

	/**
	 * \brief Delete \ref Element objects
	 * \tparam T \ref Element object
	 * \warning It is important to destroy \ref Element objects through this interface and not through a `delete`
	 * \see create
	 */
	template< class T >
	void destroy( T& );


	/**
	 * \brief Start the applicaion
	 * \return A code indicating what type of exit it is
	 * \warning This function is blocking
	 */
	ExitCode exec();

	/**
	 * \brief Get Application singleton instance
	 * \return Current Application instance
	 */
	static Application* instance();

	// PROPERTIES

	Property< bool, Application > isRunning; // read only
	const std::thread::id threadInitedIn = std::this_thread::get_id();


	void print_tree() const {

		std::cout << objectName << std::endl;

		std::function< void( const ElementTree::const_iterator& t ) > pp = [&](const ElementTree::const_iterator& t) {

					for( ElementTree::const_iterator i = t.begin();
					     i != t.end(); ++i ) {
						for( int tabs = 0; tabs < i.level(); ++tabs )
							std::cout << "\t";

						std::cout << i.data()->objectName << std::endl;

						pp( i );
					}
				};
		pp( *component_tree );
	}

private:
	Application( const Application& ) {}

	// member methods
	/// <summary>
	/// Processes the next events in the loop
	/// </summary>
	/// <returns>false if application should quit else true</returns>
	bool processEv() const;

	std::size_t getElementCount() const;
	float getDoubleClickInterval() const;
	void setDoubleClickInterval( float i );
	float getClickInterval() const;
	void setClickInterval( float i );

	// data members
	static Application* self;
	bool should_quit = false;
	bool is_running = false;

	float double_click_interval = 500.0f;
	float click_interval = 350.0f;

	ElementContainerPtr component_objects;
	ElementTreePtr component_tree;

	friend class Element;
};


template< class T, typename ... Args >
T* Application::create( Args&& ... args ) {
	static_assert(std::is_base_of< Element, T >::value, "Must be same or derived from Element");
	component_objects->push_back( std::make_unique< T >( std::forward< Args >( args )... ) );
	auto& item = component_objects->back();
	item->object = &item;

	auto item_parent = item->parent.get();

	if (item_parent) { item->internal_tree = item_parent->internal_tree.push_back(item.get()); }
	else { item->internal_tree = component_tree->push_back(item.get()); }

	item->init = true;

	return static_cast<T*>(item.get());
}

template< class T >
void Application::destroy( T& object ) {
	static_assert(std::is_convertible< T, Element* >::value, "Must be same or derived from Element");
	if( object )
		component_objects->remove( *object->object );
}

NAMESPACE_END
