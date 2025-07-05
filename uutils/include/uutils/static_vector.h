#pragma once

#include <bit>
#include <iterator>
#include <stdexcept>

namespace uutils
{
	template <class T, std::size_t Capacity>
	class StaticVector
	{
	public:
		using value_type = T;
		using size_type = std::size_t;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using Storage = std::aligned_storage_t<sizeof(T), alignof(T)>;
		
		constexpr StaticVector() noexcept : _size{ 0 } {}
		constexpr StaticVector(std::initializer_list<T> init) : _size{ 0 }
		{
			for (const T& v : init) emplace_back(v);
		}

		constexpr ~StaticVector() { clear(); }

		constexpr size_type size() const { return _size; }
		constexpr size_type capacity() const { return Capacity; }
		constexpr bool empty() const noexcept { return _size == 0; }

		constexpr reference operator[](size_type index) { return *ptr(index); }
		constexpr const_reference operator[](size_type index) const { return *ptr(index); }

		constexpr reference at(size_type index)
		{
			if (index >= _size) throw std::out_of_range("StaticVector::at");
			return (*this)[index];
		}
		constexpr const_reference at(size_type index) const
		{
			if (index >= _size) throw std::out_of_range("StaticVector::at");
			return (*this)[index];
		}

		constexpr reference front() { return (*this)[0]; }
		constexpr const_reference front() const { return (*this)[0]; }
		constexpr reference back() { return (*this)[_size - 1]; }
		constexpr const_reference back() const { return (*this)[_size - 1]; }

		template <typename... Args>
		constexpr reference emplace_back(Args&&... args)
		{
			if (_size >= Capacity)
				throw std::length_error("StaticVector capacity exceeded");
			T* p = new (static_cast<void*>(&_data[_size++])) T(std::forward<Args>(args)...);
			return *p;
		}

		constexpr void push_back(const T& value) { emplace_back(value); }
		constexpr void push_back(T&& value) { emplace_back(std::move(value)); }

		constexpr void pop_back()
		{
			if (_size == 0) return;
			--_size;
			ptr(_size)->~T();
		}

		constexpr void clear()
		{
			for (size_type i = 0; i < _size; ++i)
				ptr(i)->~T();
			_size = 0;
		}

		template <typename U>
		struct Iterator
		{
			using difference_type = std::ptrdiff_t;
			using value_type = U;
			using pointer = U*;
			using reference = U&;
			using iterator_category = std::random_access_iterator_tag;

			using RawStoragePtr = std::conditional_t<std::is_const_v<U>, const Storage*, Storage*>;
			RawStoragePtr _ptr;

			constexpr Iterator(RawStoragePtr ptr) : _ptr(ptr) {}
			constexpr reference operator*() const { return *std::bit_cast<pointer>(_ptr); }
			constexpr pointer operator->() const { return std::bit_cast<pointer>(_ptr); }
			constexpr Iterator& operator++() { ++_ptr; return *this; }
			constexpr Iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }
			constexpr Iterator& operator--() { --_ptr; return *this; }
			constexpr Iterator operator--(int) { auto tmp = *this; --(*this); return tmp; }
			constexpr Iterator operator+(difference_type n) const { return Iterator(_ptr + n); }
			constexpr Iterator operator-(difference_type n) const { return Iterator(_ptr - n); }
			constexpr difference_type operator-(const Iterator& other) const { return _ptr - other._ptr; }
			constexpr bool operator==(const Iterator& rhs) const { return _ptr == rhs._ptr; }
			constexpr bool operator!=(const Iterator& rhs) const { return _ptr != rhs._ptr; }
		};

		using iterator = Iterator<T>;
		using const_iterator = Iterator<const T>;

		constexpr iterator begin() { return iterator(&_data[0]); }
		constexpr const_iterator begin() const { return const_iterator(&_data[0]); }
		constexpr const_iterator cbegin() const { return const_iterator(&_data[0]); }

		constexpr iterator end() { return iterator(&_data[_size]); }
		constexpr const_iterator end() const { return const_iterator(&_data[_size]); }
		constexpr const_iterator cend() const { return const_iterator(&_data[_size]); }

		constexpr iterator erase(const_iterator pos)
		{
			size_type index = static_cast<size_type>(pos._ptr - &_data[0]);
			if (index >= _size)
				throw std::out_of_range("StaticVector::erase");
			ptr(index)->~T();

			for (size_type i = index; i + 1 < _size; ++i) {
				T* dst = ptr(i);
				T* src = ptr(i + 1);

				new (dst) T(std::move(*src));
				src->~T();
			}

			--_size;
			return iterator(&_data[index]);
		}
		constexpr iterator erase(iterator pos) { return erase(const_iterator(pos._ptr)); }

	private:
		std::aligned_storage_t<sizeof(T), alignof(T)> _data[Capacity];
		size_type _size;

		constexpr T* ptr(size_type index) noexcept { return std::bit_cast<T*>(&_data[index]); }
		constexpr const T* ptr(size_type index) const noexcept { return std::bit_cast<const T*>(&_data[index]); }
	};
}