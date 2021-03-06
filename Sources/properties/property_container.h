#ifndef PROPERTY_CONTAINER_H
#define PROPERTY_CONTAINER_H

#include "property_util.h"

class IProperty;

template <class T>
class Property;


/**
Properties can be added only through inherited class and should only be added at class initialization/construction.
(De)serializing of properties always happens in the same order, new properties cannot be introduced via deserialization.
**/

class PropertyContainer
{
///property container
protected:
	std::vector<IProperty*> m_props;

	void							add_property(IProperty * p);
	template <class T> Property<T>	add_property(const Property<T> & prop);
	template <class T> Property<T>	add_property(const std::string & name, const T & data);
	template <class T> Property<T>	add_property(const std::string & name, const T & data, const uint32_t flags);
	template <class T> Property<T>	add_property(const std::string & name);

public:
	PropertyContainer();
	virtual ~PropertyContainer();

	void		load_properties(const PropertyContainer & p);

	bool		has_property(const std::string & name);
	bool		has_property(const std::string & name, uint32_t type);
	IProperty*  get_property(const std::string & name); ///no throw
	bool		remove_property(const std::string & name);
	uint8_t		get_property_count();

	template <class T> Property<T> get_property(const std::string & name);
	template <class T> const Property<T> get_property(const std::string & name) const;

public:
	virtual void serialize(clan::File & file) const;
	virtual void deserialize(clan::File & file);

	virtual void net_serialize(clan::NetGameEventValue & e, bool only_changed = false) const;
	virtual void net_deserialize(const clan::NetGameEventValue & e);

	virtual void xml_serialize(clan::DomDocument doc, clan::DomElement e, bool dynamic = false) const;
	virtual void xml_deserialize(clan::DomElement e);

public:

	bool operator !=(const PropertyContainer & o) const
	{
		return this!=&o; ///FIX ME: fast, but will it give good results?
	}

///factory methods
private:
	typedef IProperty * (*prop_create_func)(const std::string &);
	static std::map<uint32_t, prop_create_func> m_prop_create;

public:
	static void register_properties();

	template <class T>
	static bool register_property()
	{
		m_prop_create[T::get_property_type_id()]=&T::create;
		return true;
	}

	static IProperty * create_property(uint32_t type, const std::string & name)
	{
		auto it = m_prop_create.find(type);

		if(it != m_prop_create.end())
			return it->second(name);

		static clan::StringFormat fmt("Property type id=%1 is not valid type id for property creation");
		fmt.set_arg(1,type);

		throw clan::Exception(fmt.get_result());
	}
};


template <class T>
Property<T> PropertyContainer::get_property(const std::string & name)
{
	auto it = std::find_if(m_props.begin(), m_props.end(), [&name](IProperty * o){return o->get_name()==name;});

	if(it!=m_props.end())
	{
		if(get_type_id<T>()!=(*it)->get_type())
			throw clan::Exception("Another property already exists with the same name but different type");

		return (*static_cast< Property<T> *>(*it));
	}

	///for now let's just throw exception
	throw clan::Exception("Tried to get non-existing property : " + name);
}

template <class T>
const Property<T> PropertyContainer::get_property(const std::string & name) const
{
	auto it = std::find_if(m_props.begin(), m_props.end(), [&name](IProperty * o){return o->get_name()==name;});

	if(it!=m_props.end())
	{
		if(get_type_id<T>()!=(*it)->get_type())
			throw clan::Exception("Another property already exists with the same name but different type");

		return (*static_cast< Property<T> *>(*it));
	}

	///for now let's just throw exception
	throw clan::Exception("Tried to get non-existing property : " + name);
}
template <class T>
Property<T> PropertyContainer::add_property(const Property<T> & prop)
{
	Property<T> * r;
	auto it = std::find_if(m_props.begin(), m_props.end(), [&prop](IProperty * o){return o->get_name()==prop.get_name();});

	if(it!=m_props.end())
	{
		if(prop.get_type()!=(*it)->get_type())
			throw clan::Exception("Another property already exists with the same name but different type");

		r = static_cast< Property<T> *>(*it);
		*r =prop; // not sure if data should be replaced or just set
		return *r;
	}

	r = new Property<T>(prop);
	m_props.push_back(r);
	return *r;
}

template <class T>
Property<T> PropertyContainer::add_property(const std::string & name)
{
	Property<T> * r;
	auto it = std::find_if(m_props.begin(), m_props.end(), [&name](IProperty * o){return o->get_name()==name;});

	if(it!=m_props.end())
	{
		r = static_cast< Property<T> *>(*it);

		if(r->get_type()!=get_type_id<T>())
			throw clan::Exception("Another property already exists with the same name but different type");

		return *r;
	}

	r = new Property<T>(name);
	m_props.push_back(r);
	return *r;
}

template <class T>
Property<T> PropertyContainer::add_property(const std::string & name, const T & data)
{
	Property<T> * r;
	auto it = std::find_if(m_props.begin(), m_props.end(), [&name](IProperty * o){return o->get_name()==name;});

	if(it!=m_props.end())
	{
		r = static_cast< Property<T> *>(*it);

		if(r->get_type()!=get_type_id<T>())
			throw clan::Exception("Another property already exists with the same name but different type");

		r->set(data); //not sure if data should be replaced or just set
		return *r;
	}

	r = new Property<T>(name,data);
	m_props.push_back(r);
	return *r;
}


template <class T>
Property<T> PropertyContainer::add_property(const std::string & name, const T & data, const uint32_t flags)
{
	Property<T> * r;
	auto it = std::find_if(m_props.begin(), m_props.end(), [&name](IProperty * o){return o->get_name()==name;});

	if(it!=m_props.end())
	{
		r = static_cast< Property<T> *>(*it);

		if(r->get_type()!=get_type_id<T>())
			throw clan::Exception("Another property already exists with the same name but different type");

		r->set(data); //not sure if data should be replaced or just set
		r->m_data->flags|=flags; ///FIX ME: ?????
		return *r;
	}

	r = new Property<T>(name,data,flags);
	m_props.push_back(r);
	return *r;
}

#endif // PROPERTY_CONTAINER_H
