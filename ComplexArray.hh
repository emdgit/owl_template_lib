#pragma once

#include "ComplexObject.hh"

#include <vector>

template <class ... Args>
class ComplexArray
{

	using value_type = ComplexObject<Args...>;

public:

	ComplexArray() = default;
	ComplexArray(size_t size) {
		array_data_.reserve(size);
	}

	void push_back(Args &&...vals) {
		array_data_.emplace_back(std::forward<Args>(vals)...);
	}

	template <class T>
	void to_user_vector(std::vector<T> &vec) const {
		if (!vec.empty()) {
			vec.clear();
		}
		vec.reserve(size());
		for (const value_type &val : array_data_) {
			vec.push_back(val.to_user_type<T>());
		}
	}

	inline size_t size() const noexcept { return array_data_.size(); }

private:

	std::vector<value_type>	array_data_;

};