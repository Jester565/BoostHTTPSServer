#pragma once
#include <string>
#include <functional>

namespace websocket
{
	class IPacket;

	class PKey
	{
	public:
		PKey(const std::string& key);

		template <typename T, typename D>
		PKey(std::string key, T obj, D func)
		{
			this->key = key;
			runMethod = std::bind(func, obj, std::placeholders::_1);
		}

		const std::string& getKey() const
		{
			return key;
		}

		virtual std::string getDescription() const
		{
			return key + ": ";
		}

		virtual void run(IPacket* iPack)
		{
			if (runMethod != nullptr)
				runMethod(iPack);
			else
				throw "Run was not overloaded or provided with a runMethod";
		}

		friend bool operator< (const PKey& pKey1, const PKey& pKey2)
		{
			return (pKey1.getKey() < pKey2.getKey());
		}

		friend bool operator> (const PKey& pKey1, const PKey& pKey2)
		{
			return (pKey1.getKey() > pKey2.getKey());
		}

		friend bool operator== (const PKey& pKey1, const PKey& pKey2)
		{
			return (pKey1.getKey() == pKey2.getKey());
		}

		friend bool operator!= (const PKey& pKey1, const PKey& pKey2)
		{
			return (pKey1.getKey() != pKey2.getKey());
		}

		friend bool operator< (const std::string& key, const PKey& pKey)
		{
			return (key < pKey.getKey());
		}

		friend bool operator> (const std::string& key, const PKey& pKey)
		{
			return (key > pKey.getKey());
		}

		friend bool operator== (const std::string& key, const PKey& pKey)
		{
			return (key == pKey.getKey());
		}

		friend bool operator != (const std::string& key, const PKey& pKey)
		{
			return (key == pKey.getKey());
		}

		virtual ~PKey();

	protected:
		std::function<void(IPacket* ipack)> runMethod;
		std::string key;
	};
}
