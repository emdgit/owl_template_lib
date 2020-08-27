#pragma once

#include <type_traits>
#include <utility>
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
	ComplexObject(const ComplexObject<Args...> &other) {
		memcpy(&data_[0], &other.data_[0], size());
	}
	ComplexObject(ComplexObject<Args...> &&other) {
		memcpy(&data_[0], &other.data_[0], size());
		memset(&other.data_[0], 0, size());
	}

	template <size_t n>
	auto& get() {
		constexpr auto offset = byte_offset<n, Args...>();
		using type = typename std::tuple_element<n, std::tuple<Args...>>::type;
		return *reinterpret_cast<type*>(&data_[offset]);
	}

	template <size_t n>
	const auto& get() const {
		constexpr auto offset = byte_offset<n, Args...>();
		using type = typename std::tuple_element<n, std::tuple<Args...>>::type;
		return *reinterpret_cast<const type*>(&data_[offset]);
	}

	static inline constexpr size_t size() noexcept {
		return sizeof...(Args);
	}

	template <class T>
	T to_user_type() const {
		static_assert(std::is_constructible_v<T,Args...>, 
			"Cannot construct to user type");
		constexpr auto seq = std::make_integer_sequence<size_t, size()>();

		return user_ctor<T>(seq);
	}

protected:

	template <class U, size_t ... vs>
	U user_ctor(std::integer_sequence<size_t, vs...> seq) const {
		return { get<vs>()... };
	}

private:
	char data_[args_size_v<Args...>];
};