#include <fstream>
#include <set>
#include <iostream>

size_t start_size = 4;

template <typename K, typename V>
class HashMap
{
    std::pair<K, V> ** arr;
    size_t capacity;
    size_t max_size;
    double k;

    void restructuring()
	{
		HashMap temp{ max_size * 2, k };

		for (size_t i = 0; i < max_size; i++)
		{
			if (arr[i] != nullptr)
			{
				temp.insert(*(arr[i]));
			}
		}
		
		for (size_t i = 0; i < max_size; i++)
		{
			if (arr[i] != nullptr)
			{
				delete arr[i];
			}
		}
		delete[] arr;
		arr = temp.arr;
		temp.arr = nullptr;
		temp.max_size = 0;
		max_size *= 2;
	}

public:

	size_t size1()
	{
		return max_size;
	}

    HashMap(double k) : arr{new std::pair<K, V> * [start_size]}, capacity{0}, max_size{start_size}, k{k}
    {   
		for (size_t i = 0; i < max_size; i++)
		{
			arr[i] = nullptr;
		}
	}

    HashMap(size_t max_size, double k) : arr{new std::pair<K, V> * [max_size]}, capacity{0}, max_size{max_size}, k{k}
    {   
		for (size_t i = 0; i < max_size; i++)
		{
			arr[i] = nullptr;
		}
	}

    HashMap(const HashMap<K, V> & other)
    {
        for (size_t i = 0; i < max_size; i++)
        {
            delete arr[i];
        }
        delete[] arr;
        arr = nullptr;
        arr = new std::pair<K, V> * [other.max_size];
        max_size = other.max_size;
        capacity = other.capacity;

        for (size_t i = 0; i < max_size; i++)
        {
            if (other.arr[i] != nullptr)
            {
                arr[i] = new std::pair<K, V>(other.arr[i][0]);
            }
            else
            {
                arr[i] = nullptr;
            }
        }
    }

    ~HashMap()
    {
        for (size_t i = 0; i < max_size; i++)
        {
            delete arr[i];
        }
        delete[] arr;
    }

    size_t size() const
    {
        return capacity;
    }

    class iterator
    {
		friend class HashMap;

        std::pair<K, V> ** arr;
		size_t max_size;
		size_t index;

    public:
        iterator(std::pair<K, V> ** arr, size_t max_size, size_t index = 0)
				 : arr{arr}
				 , max_size{max_size}
				 , index{index}
		{	}

		iterator(const iterator & other)
				 : arr{other.arr}
				 , max_size{other.max_size}
				 , index{other.index}
		{	}

		iterator operator++(int)
		{
			iterator temp{*this};
			do
			{
				index++;
				if (index == max_size)
				{
					index = 0;
				}
			} while (arr[index] == nullptr);
			return temp;
		}
	
		iterator & operator++()
		{
			do
			{
				index++;
				if (index == max_size)
				{
					index = 0;
				}
			} while (arr[index] == nullptr);
			return *this;
		}

		iterator operator--(int)
		{
			iterator temp{ *this };
			do
			{
				index--;
				if (index == -1)
				{
					index = max_size - 1;
				}
			} while (arr[index] == nullptr);
			return temp;
		}

		iterator operator--()
		{
			do
			{
				index--;
				if (index == -1)
				{
					index = max_size - 1;
				}
			} while (arr[index] == nullptr);
			return *this;
		}

		iterator & operator+=(int num)
		{
			if (num >= 0)
			{
				for (size_t i = 0; i < num; i++)
				{
					(*this)++;
				}
			}
			else
			{
				for (size_t i = 0; i < -1 * num; i++)
				{
					(*this)--;
				}
			}
			return *this;
		}

		iterator & operator-=(int num)
		{
			(*this) += -1 * num;
			return *this;
		}

		iterator operator+(int num) const
		{
			iterator temp{ *this };
			if (num >= 0)
			{
				for (size_t i = 0; i < num; i++)
				{
					temp++;
				}
			}
			else
			{
				for (size_t i = 0; i < -1 * num; i++)
				{
					temp--;
				}
			}
			return temp;
		}

		iterator operator-(int num) const
		{
			return *this + -num;
		}

		bool operator!=(const iterator other) const
		{
			return index != other.index;
		}

		bool operator==(const iterator other) const
		{
			return index == other.index;
		}

		std::pair<K, V> & operator*() const
		{
			return *(arr[index]);
		}

		std::pair<K, V> * operator->() const
		{
			return arr[index];
		}
	};

	iterator begin()
	{
		if (capacity == 0)
		{
			throw std::logic_error{"Нет элементов - нет начала."};
		}
		iterator temp{arr, max_size};
		if (arr[0] == nullptr)
		{
			temp++;
		}
		return temp;
	}

	iterator insert(std::pair<K, V> element)
	{
		if (double(capacity) / max_size > k)
		{
			restructuring();
		}

		std::hash<K> h;
		size_t index = h(element.first) % max_size;

		while (arr[index] != nullptr)
		{
			if ((*arr[index]).first == element.first)
			{
				*arr[index] = element;
				return iterator{arr, max_size, index};
			}

			index++;
			if (index == max_size)
			{
				index = 0;
			}
		}
		arr[index] = new std::pair<K, V>{element};
		capacity++;
		return iterator{arr, max_size, index};
	}

	iterator find(const K & key) const
	{
		std::hash<K> h;
		size_t index = h(key) % max_size;
		size_t temp = index;
		if (arr[index] == nullptr)
		{
			throw std::out_of_range{"Элемент не найден"};
		}
		while ((*arr[index]).first != key)
		{
			index++;
			if (index == max_size)
			{
				index = 0;
			}
			if (temp == index)
			{
				throw std::out_of_range{"Элемент не найден."};
			}
		}
		return iterator{arr, max_size, index};
	}

	void erase(const K & key)
	{
		iterator it{arr, max_size};
		try
		{
			it = find(key);
		}
		catch(const std::exception& e)
		{
			return;
		}
		
		delete arr[it.index];
		arr[it.index] = nullptr;
		capacity--;
	}
};


template <typename K, typename V>
void execution(std::istream & in, std::ostream & out)
{
	HashMap<K, V> hash_map{ 0.8 };
	size_t count_commands;
	in >> count_commands;
	for (size_t i = 0; i < count_commands; i++)
	{
		char command;
		in >> command;
		K key;
		if (command == 'A')
		{
			V value;
			in >> key >> value;
			hash_map.insert({key, value});
		}
		if (command == 'R')
		{
			in >> key;
			hash_map.erase(key);
		}
	}

	std::set<V> set_1;
	if (hash_map.size() != 0)
	{
		auto it1 = hash_map.begin();
		for (size_t i = 0; i < hash_map.size(); i++, it1++)
		{
			set_1.insert((*it1).second);
		}
	}

	out << hash_map.size() << " " << set_1.size();
}

int main()
{
	std::ifstream fin{ "input.txt" };
	std::ofstream fout{ "output.txt" };
	
	char type_key, type_value;
	fin >> type_key >> type_value;

	if (type_key == 'I' && type_value == 'I')
	{
		execution<int, int>(fin, fout);
	}

	if (type_key == 'I' && type_value == 'D')
	{
		execution<int, double>(fin, fout);
	}

	if (type_key == 'I' && type_value == 'S')
	{
		execution<int, std::string>(fin, fout);
	}

	if (type_key == 'D' && type_value == 'I')
	{
		execution<double, int>(fin, fout);
	}

	if (type_key == 'D' && type_value == 'D')
	{
		execution<double, double>(fin, fout);
	}

	if (type_key == 'D' && type_value == 'S')
	{
		execution<double, std::string>(fin, fout);
	}

	if (type_key == 'S' && type_value == 'I')
	{
		execution<std::string, int>(fin, fout);
	}

	if (type_key == 'S' && type_value == 'D')
	{
		execution<std::string, double>(fin, fout);
	}

	if (type_key == 'S' && type_value == 'S')
	{
		execution<std::string, std::string>(fin, fout);
	}
}
