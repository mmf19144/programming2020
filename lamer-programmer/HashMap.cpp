#include <iostream>
#include <functional>
#include <fstream>

// почти полностью переписал реализацию
template<typename K, typename V>
class HashMap
{
	std::pair<K, V> ** arr;
	size_t current_size;
	size_t capacity;
	double percentage_of_occupancy;

	void restructuring()
	{
		size_t temp_capacity;
		if (capacity == 0)
		{
			temp_capacity = 1;
		}
		else
		{
			temp_capacity = capacity * 2;
		}
		HashMap<K, V> temp{percentage_of_occupancy, temp_capacity};
		for (const auto & element : *this)
		{
			temp.insert(element.first, element.second);
		}
		*this = temp;
	}

	size_t get_index(const K & key) const
	{
		std::hash<K> h;
		return h(key) % capacity;
	}

	size_t next_index(size_t index) const
	{
		index++;
		if (index == capacity)
		{
			index = 0;
		}
		return index;
	}

public:
	explicit HashMap(double percentage_of_occupancy)
			: percentage_of_occupancy{percentage_of_occupancy}
			, capacity{2}
			, current_size{0}
	{
		arr = new std::pair<K, V> * [capacity];
		for (size_t i = 0; i < capacity; i++)
		{
			arr[i] = nullptr;
		}
	}

	HashMap(double percentage_of_occupancy, size_t start_size)
			: percentage_of_occupancy{percentage_of_occupancy}
			, capacity{start_size}
			, current_size{0}
	{
		arr = new std::pair<K, V> * [capacity];
		for (size_t i = 0; i < capacity; i++)
		{
			arr[i] = nullptr;
		}
	}

	HashMap(const HashMap<K, V> & other)
			: percentage_of_occupancy{other.percentage_of_occupancy}
	{
		current_size = other.current_size;
		capacity = other.capacity;
		arr = new std::pair<K, V> * [capacity];
		for (size_t i = 0; i < capacity; i++)
		{
			if (other.arr[i] == nullptr)
			{
				arr[i] = nullptr;
			} else
			{
				arr[i] = new std::pair<K, V>{*(other.arr[i])};
			}
		}
	}

	~HashMap()
	{
		for (size_t i = 0; i < capacity; i++)
		{
			delete arr[i];
		}
		delete[] arr;
	}

	HashMap<K, V> & operator=(const HashMap<K, V> & other)
	{
		if (this == &other)
		{
			return *this;
		}
		percentage_of_occupancy = other.percentage_of_occupancy;
		for (size_t i = 0; i < capacity; i++)
		{
			delete arr[i];
		}
		delete[] arr;
		arr = nullptr;

		current_size = other.current_size;
		capacity = other.capacity;
		arr = new std::pair<K, V> * [other.capacity];
		for (size_t i = 0; i < capacity; i++)
		{
			if (other.arr[i] == nullptr)
			{
				arr[i] = nullptr;
			} else
			{
				arr[i] = new std::pair<K, V>{*(other.arr[i])};
			}
		}
		return *this;
	}

	class iterator
	{
		friend class HashMap;

		std::pair<K, V> ** arr;
		size_t capacity;
		size_t index;

	public:
		iterator()
				: arr{nullptr}
				, capacity{0}
				, index{0}
		{   }

		iterator(std::pair<K, V> ** arr, size_t capacity, size_t index)
				: arr{arr}
				, capacity{capacity}
				, index{index}
		{   }

		iterator(const iterator & other)
				: arr{other.arr}
				, capacity{other.capacity}
				, index{other.index}
		{   }

		std::pair<K, V> & operator*() const
		{
			if (index == capacity)
			{
				throw std::out_of_range{"Разыменование итератора end()"};
			}
			return *(arr[index]);
		}

		iterator & operator++()
		{
			do
			{
				if (index == capacity)
				{
					throw std::out_of_range{"HashMap::iterator::operator++() - выход за пределы таблицы"};
				}
				index++;
			} while (arr[index] == nullptr);
			return *this;
		}

		const iterator operator++(int)
		{
			iterator temp{*this};
			do
			{
				if (index == capacity)
				{
					throw std::out_of_range{"HashMap::iterator::operator++(int) - выход за пределы таблицы"};
				}
				index++;
			} while (arr[index] == nullptr);
			return temp;
		}

		iterator & operator=(const iterator & other)
		{
			if (this == &other)
			{
				return *this;
			}
			arr = other.arr;
			capacity = other.capacity;
			index = other.index;
		}

		bool operator==(const iterator & other) const
		{
			return index == other.index && arr == other.arr;
		}

		bool operator!=(const iterator & other) const
		{
			return index != other.index || arr != other.arr;
		}
	};

	iterator begin() const
	{
		HashMap::iterator temp{arr, capacity, 0};
		if (arr[0] == nullptr)
		{
			temp++;
		}
		return temp;
	}

	iterator end() const
	{
		return HashMap::iterator{arr, capacity, capacity};
	}

	iterator find(const K & key) const
	{
		size_t index = get_index(key);
		size_t start_index = index;
		while (true)
		{
			if (arr[index] == nullptr)
			{
				index = next_index(index);
				if (start_index == index)
				{
					return end();
				}
				continue;
			}
			else
			{
				if (arr[index]->first != key)
				{
					if (start_index == index)
					{
						return end();
					}
					index = next_index(index);
					continue;
				}
			}
			break;
		}
		return iterator(arr, capacity, index);
	}

	void insert(const K & key, const V & value)
	{
		if (double(current_size) / capacity >= percentage_of_occupancy || capacity == 0)
		{
			restructuring();
		}
		size_t index = get_index(key);
		while (arr[index] != nullptr)
		{
			if (arr[index]->first == key)
			{
				arr[index]->second = value;
				return;
			}
			index++;
			if (index == capacity)
			{
				index = 0;
			}
		}
		arr[index] = new std::pair<K, V>{key, value};
		current_size++;
	}

	void erase(const K & key)
	{
		iterator it = find(key);

		if (it == end())
		{
			return;
		}

		delete arr[it.index];
		arr[it.index] = nullptr;
		current_size--;
	}

	size_t size() const
	{
		return current_size;
	}
};

template<typename K, typename V>
size_t get_unique_count(HashMap<K, V> & hash_table)
{
	HashMap<V, int> temp{0.8};
	for (const auto & p : hash_table)
	{
		temp.insert(p.second, 0);
	}
	return temp.size();
}

template<typename K, typename V>
void foo2(std::ifstream & fin, std::ofstream & fout)
{
	HashMap<K, V> hash_map{0.8};
	size_t count_commands;
	fin >> count_commands;

	for (size_t i = 0; i < count_commands; i++)
	{
		char command;
		K key;
		fin >> command;
		if (command == 'A')
		{
			V value;
			fin >> key >> value;
			hash_map.insert(key, value);
		}
		if (command == 'R')
		{
			fin >> key;
			hash_map.erase(key);
		}
	}
	fout << hash_map.size() << " ";
	fout << get_unique_count<K, V>(hash_map) << std::endl;
}

template<typename K>
void foo1(std::ifstream & fin, std::ofstream & fout)
{
	char type_value;
	fin >> type_value;

	if (type_value == 'I')
	{
		foo2<K, int>(fin, fout);
	}
	if (type_value == 'D')
	{
		foo2<K, double>(fin, fout);
	}
	if (type_value == 'S')
	{
		foo2<K, std::string>(fin, fout);
	}
}

int main()
{
	std::ifstream fin{"input.txt"};
	std::ofstream fout{"output.txt"};

	char type_key;
	fin >> type_key;

	if (type_key == 'I')
	{
		foo1<int>(fin, fout);
	}
	if (type_key == 'D')
	{
		foo1<double>(fin, fout);
	}
	if (type_key == 'S')
	{
		foo1<std::string>(fin, fout);
	}
}
