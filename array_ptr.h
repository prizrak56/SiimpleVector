#pragma once
#include <algorithm>
#include <cassert>
#include <cstdlib>

template <typename Type>
class ArrPtr {
public:

	ArrPtr() = default;

	explicit ArrPtr(size_t size) {
		if (size == 0) {
			raw_ptr_ = nullptr;
		}
		else {
			raw_ptr_ = new Type [size];
		}
	}
	explicit ArrPtr(Type* raw_ptr) noexcept : raw_ptr_(raw_ptr) {}

	~ArrPtr() {
		delete[] raw_ptr_;
	}

	Type& operator[](size_t index) noexcept {

		return raw_ptr_[index];
	}
	const Type& operator[](size_t index) const noexcept {
		return raw_ptr_[index];
	}

	explicit operator bool() noexcept {
		if (raw_ptr_ != nullptr) {
			return true;
		}
		else {
			return false;
		}
	}

	Type* Get_begin() noexcept {
		return &raw_ptr_[0];
	}

	void swap(ArrPtr &other) noexcept {
		std::swap(other.raw_ptr_, raw_ptr_);
	}

private:
	Type* raw_ptr_ = nullptr;
};
