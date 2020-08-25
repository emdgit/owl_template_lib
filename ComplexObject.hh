#pragma once

#include <type_traits>
#include <new>

/// Calculate how much bytes pack weighs
template <class ...Args>
static constexpr size_t args_size_v = (sizeof(Args) + ...);

/// Get how much bytes stand before n's type in pack
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

/// Composite object, made of given data
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

	template <size_t n>
	auto& get() {
		constexpr auto offset = byte_offset<n, Args...>();
		using type = typename std::tuple_element<n, std::tuple<Args...>>::type;
		return *reinterpret_cast<type*>(&data_[offset]);
	}

	inline constexpr size_t size() const noexcept {
		return sizeof...(Args);
	}

private:
	char data_[args_size_v<Args...>];
};