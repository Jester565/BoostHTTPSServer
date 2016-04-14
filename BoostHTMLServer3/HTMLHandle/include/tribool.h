#ifndef TRIBOOL_HPP
#define TRIBOOL_HPP

namespace websocket
{
	struct Indeterminate
	{

	};

	static Indeterminate indeterminate;

	class tribool
	{
	public:
		tribool()
			: indeterm(true), tf(false)
		{

		}


		tribool(bool state)
			: indeterm(false), tf(state)
		{

		}

		tribool(Indeterminate i)
			: indeterm(true), tf(false)
		{

		}

		bool is_indeterminate() const
		{
			return indeterm;
		}

		operator bool() const
		{
			if (!indeterm)
			{
				return tf;
			}
			return false;
		}

		bool const operator ! () const
		{
			if (!indeterm)
			{
				return !tf;
			}
			return false;
		}

		friend bool operator == (const tribool& t1, const tribool& t2)
		{
			if (t1.indeterm && t2.indeterm)
			{
				return true;
			}
			else if (t1.indeterm != t2.indeterm)
			{
				return false;
			}
			else
			{
				return (t1.tf == t2.tf);
			}
		}

		friend bool operator != (const tribool& t1, const tribool& t2)
		{
			return !(t1 == t2);
		}

		friend bool operator == (const tribool& t1, const bool& b)
		{
			if (t1.indeterm)
			{
				return false;
			}
			return (t1.tf == b);
		}

		friend bool operator != (const tribool& t1, const bool& b)
		{
			return !(t1 == b);
		}

		tribool& operator=(const Indeterminate& indeterminate)
		{
			indeterm = true;
			return *this;
		}

		tribool& operator=(const tribool& t)
		{
			indeterm = t.indeterm;
			tf = t.tf;
			return *this;
		}

		tribool& operator=(const bool& b)
		{
			indeterm = false;
			tf = b;
			return *this;
		}

		~tribool()
		{

		}

	private:
		bool indeterm;
		bool tf;
	};

	static bool is_indeterminate(const tribool& t)
	{
		return t.is_indeterminate();
	}
}

#endif