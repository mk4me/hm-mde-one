#ifndef SIMPLE_FUNCTORS
#define SIMPLE_FUNCTORS


// few classes representing functors calling member functions
 

//////////////////////////////////////////////////////////////////////////
// simple 'interface'
class ISimpleNoArgFunctor
{
public:

	virtual void operator()()		{};
};


//////////////////////////////////////////////////////////////////////////
// simple 'interface'
template<class Arg>
class ISimpleOneArgFunctor
{
public:

	virtual void operator()(Arg)	{};
};


//////////////////////////////////////////////////////////////////////////
// functor calling no argument functions returning void
template<class T>
class CSimpleNoArgFunctor : public ISimpleNoArgFunctor
{

	// object at which we are calling function
	T* _object;

	// pointer to interesting us function
	void (T::*_fun)();

public:

	T*	getObject()	{ return _object; }

	// c - tor
	CSimpleNoArgFunctor(T* object, void (T::*fun)())
		: _object(object), _fun(fun)
	{}

	// run
	virtual void operator()()
	{
		return (_object->*_fun)();
	}

	// ==
	bool operator==(CSimpleNoArgFunctor& other)
	{
		return (_object == other._object && _fun == other._fun);
	}
};


//////////////////////////////////////////////////////////////////////////
// functor calling one argument functions returning void
template<class T, class Arg>
class CSimpleOneArgFunctor : public ISimpleOneArgFunctor<Arg>
{
	// object at which we are calling function
	T* _object;

	// pointer to interesting us function
	void (T::*_fun)(Arg);

public:

	T*	getObject()	{ return _object; }

	// c - tor
	CSimpleOneArgFunctor(T* object, void (T::*fun)(Arg))
		: _object(object), _fun(fun)
	{}

	// run
	virtual void operator()(Arg argument)
	{
		return (_object->*_fun)(argument);
	}

	// ==
	bool operator==(CSimpleOneArgFunctor& other)
	{
		return (_object == other._object && _fun == other._fun);
	}
};


#endif // SIMPLE_FUNCTORS