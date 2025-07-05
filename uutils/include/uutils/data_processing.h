#pragma once

#include <type_traits>
#include <functional>
#include <iostream>
#include <vector>

namespace uutils::data_processing
{
	class Range;
	namespace detail
	{
		template <class T>
		concept Iterable = requires(T & t)
		{
			std::begin(t);
			std::end(t);
		};

		template <class F, class Arg>
		concept PredFunc = requires(F && f, Arg && arg)
		{
			requires !std::is_void_v<std::invoke_result_t<F, Arg>>;
		};

		template <class F, class Arg>
		concept TerminatorFunc = requires(F && f, Arg && arg)
		{
			requires std::is_void_v<std::invoke_result_t<F, Arg>>;
		};


		template <class TIterator>
		class TRange
		{
		public:

			constexpr TIterator begin() const { return _begin; }
			constexpr TIterator end() const { return _end; }

		private:
			TIterator _begin;
			TIterator _end;

			constexpr TRange(TIterator begin, TIterator end)
				: _begin(begin), _end(end) {
			}

			friend class ::uutils::data_processing::Range;
		};

		template <class TRange, class Func>
		class TMap
		{
		public:
			class Iterator
			{
			public:
				using It = decltype(std::declval<TRange>().begin());
				constexpr Iterator(It it, Func func) : _it(it), _func(func) {}

				constexpr auto operator*() const { return _func(*_it); }
				constexpr Iterator& operator++() { ++_it; return *this; }
				constexpr Iterator& operator--() { --_it; return *this; }
				constexpr bool operator!=(const Iterator& other) const { return _it != other._it; }

			private:
				It _it;
				Func _func;
			};

			constexpr TMap(TRange range, Func func)
				: _range(std::move(range)), _func(func) {
			}

			constexpr Iterator begin() const { return Iterator(_range.begin(), _func); }
			constexpr Iterator end() const { return Iterator(_range.end(), _func); }

		private:
			TRange _range;
			Func _func;
		};

		template <typename TRange, class Func>
		constexpr auto map_impl(TRange&& range, Func func)
		{
			return TMap<TRange, Func>(std::forward<TRange>(range), func);
		}

		template <class TRange, class Func>
		class TFilter
		{
		public:
			class Iterator
			{
			public:
				using It = decltype(std::declval<TRange>().begin());
				constexpr Iterator(It it, It end, It begin, Func func)
					: _it(it), _end(end), _begin(begin), _func(func) {
					advance();
				}

				constexpr auto operator*() const { return *_it; }
				constexpr Iterator& operator++()
				{
					++_it;
					advance();
					return *this;
				}
				constexpr Iterator& operator--()
				{
					reverse();
					return *this;
				}
				constexpr bool operator!=(const Iterator& other) const { return _it != other._it; }

			private:
				It _it;
				It _end;
				It _begin;
				Func _func;

				constexpr void advance()
				{
					while (_it != _end && !_func(*_it)) ++_it;
				}
				constexpr void reverse()
				{
					while (_it != _begin && !_func(*_it)) --_it;
				}
			};

			constexpr TFilter(TRange range, Func func)
				: _range(range), _func(func) {
			}

			constexpr Iterator begin() const { return Iterator(_range.begin(), _range.end(), _range.begin(), _func); }
			constexpr Iterator end() const { return Iterator(_range.end(), _range.end(), _range.begin(), _func); }

		private:
			TRange _range;
			Func _func;
		};

		template <typename TRange, class Func>
		constexpr auto filter_impl(TRange&& range, Func pred)
		{
			return TFilter<TRange, Func>(std::forward<TRange>(range), pred);
		}

		template <class TRange, std::integral TNumber>
		class TSkip
		{
		public:
			class Iterator
			{
			public:
				using It = decltype(std::declval<TRange>().begin());
				constexpr Iterator(It it, It end, TNumber skip)
					: _it(it)
				{
					for (TNumber i = static_cast<TNumber>(0); _it != end && i < skip; i++)
					{
						++_it;
					}
				}

				constexpr auto operator*() const { return *_it; }
				constexpr Iterator& operator++() { ++_it; return *this; }
				constexpr Iterator& operator--() { --_it; return *this; }
				constexpr bool operator!=(const Iterator& other) const { return _it != other._it; }

			private:
				It _it;
			};

			constexpr TSkip(TRange range, TNumber skip)
				: _range(range), _skip(skip) {
			}

			constexpr Iterator begin() const { return Iterator(_range.begin(), _range.end(), _skip); }
			constexpr Iterator end() const { return Iterator(_range.end(), _range.end(), 0); }

		private:
			TRange _range;
			TNumber _skip;
		};

		template <typename TRange, std::integral TNumber>
		constexpr auto skip_impl(TRange&& range, TNumber skip)
		{
			return TSkip<TRange, TNumber>(std::forward<TRange>(range), skip);
		}

		template <class TRange, std::integral TNumber>
		class TTake
		{
		public:
			class Iterator
			{
			public:
				using It = decltype(std::declval<TRange>().begin());
				constexpr Iterator(It it, It end, TNumber amount)
					: _it(it)
				{
					for (TNumber i = static_cast<TNumber>(0); _it != end && i < amount; i++)
					{
						++_it;
					}
				}

				constexpr auto operator*() const { return *_it; }
				constexpr Iterator& operator++() { ++_it; return *this; }
				constexpr Iterator& operator--() { --_it; return *this; }
				constexpr bool operator!=(const Iterator& other) const { return _it != other._it; }

			private:
				It _it;
			};

			constexpr TTake(TRange range, TNumber amount)
				: _range(range), _amount(amount) {
			}

			constexpr Iterator begin() const { return Iterator(_range.begin(), _range.end(), 0); }
			constexpr Iterator end() const { return Iterator(_range.begin(), _range.end(), _amount); }

		private:
			TRange _range;
			TNumber _amount;
		};

		template <typename TRange, std::integral TNumber>
		constexpr auto take_impl(TRange&& range, TNumber amount)
		{
			return TTake<TRange, TNumber>(std::forward<TRange>(range), amount);
		}

		template <class TRange>
		class TReverse
		{
		public:
			class Iterator
			{
			public:
				using It = decltype(std::declval<TRange>().begin());
				constexpr Iterator(It it) : _it(it) {}

				constexpr auto operator*() const { return *_it; }
				constexpr Iterator& operator++() { --_it; return *this; }
				constexpr Iterator& operator--() { ++_it; return *this; }
				constexpr bool operator!=(const Iterator& other) const { return _it != other._it; }

			private:
				It _it;
			};

			constexpr TReverse(TRange range) : _range(range) {}

			constexpr Iterator begin() const { return Iterator(--_range.end()); }
			constexpr Iterator end() const { return Iterator(--_range.begin()); }

		private:
			TRange _range;
		};

		template <typename TRange>
		constexpr auto reverse_impl(TRange&& range)
		{
			return TReverse<TRange>(std::forward<TRange>(range));
		}

		template <class T>
		class TEnumerate
		{
		public:
			class Iterator
			{
			public:
				constexpr Iterator(T it) : _it(it) {}

				constexpr auto operator*() const { return _it; }
				constexpr Iterator& operator++() { ++_it; return *this; }
				constexpr Iterator& operator--() { --_it; return *this; }
				constexpr bool operator!=(const Iterator& other) const { return _it != other._it; }

			private:
				T _it;
			};

			constexpr TEnumerate(T min_, T max_)
				: _min(min_), _max(max_) {
			}

			constexpr Iterator begin() const { return Iterator(_min); }
			constexpr Iterator end() const { return Iterator(_max); }

		private:
			T _min;
			T _max;
		};


		template <typename TRange>
		constexpr auto to_vector_impl(TRange&& range)
		{
			std::vector<std::decay_t<decltype(*range.begin())>> result;
			for (auto&& item : range)
				result.push_back(item);
			return result;
		}

		template <typename TRange>
		constexpr void print_impl(TRange&& range)
		{
			for (auto&& item : range)
			{
				std::cout << item << " ";
			}
		}

		template <typename TRange>
		constexpr auto sum_impl(TRange&& range)
		{
			auto sum = static_cast<std::remove_all_extents_t<decltype(*range.begin())>>(0);
			for (auto&& item : range)
			{
				sum += item;
			}
			return sum;
		}

		template <typename TRange, typename Func>
		constexpr auto all_impl(TRange&& range, Func&& pred)
		{
			for (auto&& item : range)
			{
				if (!pred(item)) return false;
			}
			return true;
		}

		template <typename TRange, typename Func>
		constexpr auto any_impl(TRange&& range, Func&& pred)
		{
			for (auto&& item : range)
			{
				if (pred(item)) return true;
			}
			return false;
		}

		template <typename TRange, typename Func>
		constexpr auto none_impl(TRange&& range, Func&& pred)
		{
			for (auto&& item : range)
			{
				if (pred(item)) return false;
			}
			return true;
		}
	}

	class Range
	{
	public:
		constexpr static auto from(const detail::Iterable auto& iterable)
		{
			return detail::TRange{ std::begin(iterable), std::end(iterable) };
		}
	private:
		constexpr Range() = default;
	};

	template <typename Range, typename Func>
		requires detail::PredFunc<Func, Range>
	constexpr auto operator>(Range&& range, Func&& func)
	{
		return std::invoke(std::forward<Func>(func), std::forward<Range>(range));
	}

	template <typename Range, typename Func>
		requires detail::TerminatorFunc<Func, Range>
	constexpr void operator>(Range&& range, Func&& func)
	{
		std::invoke(std::forward<Func>(func), std::forward<Range>(range));
	}

	constexpr auto map(auto pred) { return [=](auto&& range) { return detail::map_impl(std::forward<decltype(range)>(range), pred); }; }
	constexpr auto filter(auto pred) { return [=](auto&& range) { return detail::filter_impl(std::forward<decltype(range)>(range), pred); }; }
	constexpr auto skip(std::integral auto skip) { return [=](auto&& range) { return detail::skip_impl(std::forward<decltype(range)>(range), skip); }; }
	constexpr auto take(std::integral auto amount) { return [=](auto&& range) { return detail::take_impl(std::forward<decltype(range)>(range), amount); }; }
	constexpr auto reverse() { return [=](auto&& range) { return detail::reverse_impl(std::forward<decltype(range)>(range)); }; }
	template <std::integral T> constexpr auto range(T from, T count) { return detail::TEnumerate(from, from + count); }

	constexpr auto to_vector() { return[=](auto&& range) { return detail::to_vector_impl(range); }; }
	constexpr auto print() { return[=](auto&& range) { detail::print_impl(range); }; }
	constexpr auto sum() { return [=](auto&& range) { return detail::sum_impl(range); }; }
	constexpr auto all(auto&& func) { return [=](auto&& range) { return detail::all_impl(range, func); }; }
	constexpr auto any(auto&& func) { return [=](auto&& range) { return detail::any_impl(range, func); }; }
	constexpr auto none(auto&& func) { return [=](auto&& range) { return detail::none_impl(range, func); }; }
}