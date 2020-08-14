#pragma once

#include <type_traits>
#include <new>
//#include <iostream>

template <class ...Args>
static constexpr size_t args_size_v = (sizeof(Args) + ...);

template <size_t n, class T, class ... Args>
static constexpr size_t byte_offset() noexcept
{
	static_assert(n <= sizeof...(Args), "Index is out of pack's range");
	if constexpr (n == 0) {
		return 0;
	}
	else {
		return sizeof(T) + byte_offset<n - 1, Args...>();
	}
}

template <class ... Args>
class ComplexObject {
public:
	ComplexObject() = delete;
	explicit ComplexObject(Args &&...args) {
		int i(0);

		auto fill = [&](auto &&val) {
			using val_t = typename std::remove_reference_t<decltype(val)>;
			new (&data_[i]) val_t(val);
			i += sizeof(val);
		};

		(fill(std::forward<Args>(args)), ...);
	}

	auto get(size_t n) {

	}

private:
	char data_[args_size_v<Args...>];
	
};