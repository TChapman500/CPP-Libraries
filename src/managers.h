#pragma once
#include <vector>
#include <string>
#include <memory>

namespace TChapman500 {

template<typename T> class Manager
{
public:
	static inline std::shared_ptr<T> GetItem(std::wstring name)
	{
		for (const std::pair<std::wstring, std::shared_ptr<T>> &item : _Items)
		{
			if (item.first == name)
				return item.second;
		}
		return nullptr;
	}
	
	static inline bool AddItem(std::wstring name, std::shared_ptr<T> &item)
	{
		for (const std::pair<std::wstring, std::shared_ptr<T>> &xItem : _Items)
		{
			if (xItem.first == name)
				return false;
		}
		
		_Items.push_back(std::pair<std::wstring, std::shared_ptr<T>>(name, item));
		return true;
	}
	
	static inline bool RemoveItem(std::wstring name)
	{
		for (size_t i = 0; i < _Items.size(); i++)
		{
			const std::pair<std::wstring, std::shared_ptr<T>> &xItem = _Items[i];
			if (xItem.name == name)
			{
				_Items.erase(_Items.begin() + i);
				return true;
			}
		}
		return false;
	}
	
private:
	static std::vector<std::pair<std::wstring, std::shared_ptr<T>>> _Items;
};

template<typename T>
std::vector<std::pair<std::wstring, std::shared_ptr<T>>> Manager<T>::_Items;

}
