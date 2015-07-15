#pragma once

/// \brief A class to manage resources acquisition and an optional freeing.
///
/// The idea of this class is taken from Andrei Alexandrescu presentation at CH9:
/// http://channel9.msdn.com/Shows/Going+Deep/C-and-Beyond-2012-Andrei-Alexandrescu-Systematic-Error-Handling-in-C
/// It is a revised version of his earlier work, which can be found here:
/// http://www.drdobbs.com/cpp/generic-change-the-way-you-write-excepti/184403758
/// The point is to use the RAII idiom to be able to free the resources when the scope is exited.
/// The key point though is the ability to cancel the resources freeing.
/// Basically, this means that it is possible to rollback any changes if, for example, an exception is thrown.
/// C++11 made this class very handy to use.
///
template <class Fun>
class ScopeGuard
{

public:

	ScopeGuard(Fun fun)
		: m_fun(std::move(fun))
		, m_active(true)
	{}

	ScopeGuard(ScopeGuard&& rhs)
		: m_fun(std::move(rhs.m_fun))
		, m_active(rhs.m_active)
	{
		rhs.dismiss(); // tell the other object, that it can forget about its duties, because our object will do all the work now
	}

	/// \brief A destructor.
	///
	/// The destructor uses the m_active flag to decide whether
	/// to it is needed to release the resource or not.
	///
	~ScopeGuard()
	{
		if (m_active)
		{
			m_fun();
		}
	}

	ScopeGuard() = delete;
	ScopeGuard(ScopeGuard const&) = delete;
	ScopeGuard& operator=(ScopeGuard const&) = delete;

	/// \brief A method to clear the release flag.
	///
	/// The method simply sets m_active to false, thus, telling
	/// the object to cancel the resource freeing during destruction.
	///
	void dismiss()
	{
		m_active = false;
	}

private:

	/// \brief A function to free the resources.
	///
	/// This function is actually a closure created by a lambda expression.
	/// Therefore, it contains all the required data to be released.
	/// \sa ScopeGuard()
	///
	Fun m_fun;

	/// \brief A release flag.
	///
	/// The flag shows whether the resource is to be released or not.
	/// \sa dismiss()
	/// \sa ~ScopeGuard()
	///
	bool m_active;

};

/// \brief A helping function to construct a ScopeGuard object
///
/// \param A lambda expression.
/// \return A constructed ScopeGuard object.
///
template <class Fun>
ScopeGuard<Fun> guardScope(Fun fun)
{
	return ScopeGuard<Fun>(std::move(fun));
}
