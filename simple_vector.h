#pragma once
#include "array_ptr.h"
#include <initializer_list>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <iterator>


//�� �������� �������� ��������� �� ������ ������� � ������� �������.��� 
//���������� ������������ � ��������������� ��� ����������� 
//�������������� ����� - ������, ����� ���������� ��� ����������� � ������� 
//���������� �����������.���� ����������� ������ ��������� �� ��������
//������ ����� ������ - ������.����� �������(�� �����!) ����� ����� ��������� ��������� :
class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity_to_reserve)
        : capacity_(capacity_to_reserve)
    {
    }

    size_t GetCapacity() {
        return capacity_;
    }

private:
    size_t capacity_;
};



template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // ������ ������ �� size ���������, ������������������ ��������� �� ���������
    explicit SimpleVector(size_t size) : SimpleVector(size, Type()) {
    }

    // ������ ������ �� size ���������, ������������������ ��������� value
    SimpleVector(size_t size, const Type& value) : value_(size), size_(size), capacite_(size) {
        std::fill(value_.Get_begin(), value_.Get_begin() + size, value);
    }

    // ������ ������ �� std::initializer_list
    SimpleVector(std::initializer_list<Type> init) : value_(init.size()), size_(init.size()), capacite_(init.size()) {
        std::copy(init.begin(), init.end(), value_.Get_begin());
    }

    SimpleVector(SimpleVector&& other) : value_(other.capacity_) {
        swap(other);
    }

    SimpleVector(ReserveProxyObj reserve) {
        Reserve(reserve.GetCapacity());
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacite_) {
            ArrPtr<Type> tmp(new_capacity);
            std::fill(tmp.Get_begin(), tmp.Get_begin() + new_capacity, Type());
            std::copy(value_.Get_begin(), value_.Get_begin() + size_, tmp.Get_begin());
            value_.swap(tmp);
            capacite_ = new_capacity;
        }
    }

    // ���������� ���������� ��������� � �������
    size_t GetSize() const noexcept {
        return size_;
    }

    // ���������� ����������� �������
    size_t GetCapacity() const noexcept {
        return capacite_;
    }

    // ��������, ������ �� ������
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // ���������� ������ �� ������� � �������� index
    Type& operator[](size_t index) noexcept {
        return value_[index];
    }

    // ���������� ����������� ������ �� ������� � �������� index
    const Type& operator[](size_t index) const noexcept {
        return value_[index];
    }

    // ���������� ����������� ������ �� ������� � �������� index
    // ����������� ���������� std::out_of_range, ���� index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("out_of_range");
        }
        else {
            return value_[index];
        }
    }

    // ���������� ����������� ������ �� ������� � �������� index
    // ����������� ���������� std::out_of_range, ���� index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("out_of_range");
        }
        else {
            return value_[index];
        }
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Fill(Iterator begin_fill, Iterator end_fill) {
        assert(begin_fill < end_fill);                               
        for (; begin_fill != end_fill; ++begin_fill) {                
            *begin_fill = std::move(Type());                           
        }
    }

    void Resize(size_t new_size) {
        if (new_size <= capacite_) {
            if (new_size < size_) {
                size_ = new_size;
            }
            else {
                Fill(value_.Get_begin(), value_.Get_begin() + size_ + new_size);
            }
        }
        else {
            size_t new_capacity = std::max(new_size, capacite_ * 2);
            ArrPtr<Type> new_array(new_capacity);

            Fill(new_array.Get_begin(), new_array.Get_begin() + capacite_);

            std::move(value_.Get_begin(), value_.Get_begin() + new_capacity, new_array.Get_begin());

            value_.swap(new_array);
            size_ = new_size;
            capacite_ = new_capacity;
        }
    }

    // ���������� �������� �� ������ �������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    Iterator begin() noexcept {
        return value_.Get_begin();
    }

    // ���������� �������� �� �������, ��������� �� ���������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    Iterator end() noexcept {
        return value_.Get_begin() + size_;
    }

    // ���������� ����������� �������� �� ������ �������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    ConstIterator begin() const noexcept {
        return value_.Get_begin();
    }

    // ���������� �������� �� �������, ��������� �� ���������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    ConstIterator end() const noexcept {
        return value_.Get_begin() + size_;
    }

    // ���������� ����������� �������� �� ������ �������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    ConstIterator cbegin() const noexcept {
        return value_.Get_begin();
    }

    // ���������� �������� �� �������, ��������� �� ���������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    ConstIterator cend() const noexcept {
        return value_.Get_begin() + size_;
    }

    SimpleVector(const SimpleVector& other) : value_(other.capacite_), size_(other.size_) {

        std::copy(other.begin(), other.end(), value_.Get_begin());

    }

    SimpleVector& operator=(const SimpleVector& rhs) {

        if (&value_ != &rhs.value_) {

            ArrPtr<Type> tmp(rhs.GetCapacity());
            std::copy(rhs.begin(), rhs.end(), tmp.Get_begin());
            value_.swap(tmp);
            size_ = rhs.size_;
            capacite_ = rhs.capacite_;
        }
        return *this;
    }

    // ��������� ������� � ����� �������
    // ��� �������� ����� ����������� ����� ����������� �������
    void PushBack(const Type& item) {
        if (size_ + 1 > capacite_) {
            size_t tmp_c = std::max(size_ + 1, capacite_ * 2);
            ArrPtr<Type> tmp(tmp_c);
            std::move(value_.Get_begin(), value_.Get_begin() + size_, tmp.Get_begin());
            value_.swap(tmp);
            capacite_ = tmp_c;
        }
        value_[size_] = std::move(item);
        ++size_;

    }

    // ��������� �������� value � ������� pos.
    // ���������� �������� �� ����������� ��������
    // ���� ����� �������� �������� ������ ��� �������� ���������,
    // ����������� ������� ������ ����������� �����, � ��� ������� ������������ 0 ����� ������ 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        size_t quantity = pos - value_.Get_begin();
        if (capacite_ == 0) {
            ArrPtr<Type>tmp(1);
            tmp[quantity] = value;
            value_.swap(tmp);
            ++capacite_;
        }
        else if (size_ < capacite_) {
            std::move_backward(value_.Get_begin() + quantity, value_.Get_begin() + size_, value_.Get_begin() + size_ + 1);
            value_[quantity] = std::move(value);
        }
        else {
            size_t tmp_c = std::max(size_ + 1, capacite_ * 2);
            ArrPtr<Type> tmp(tmp_c);
            std::move(value_.Get_begin(), value_.Get_begin() + size_, tmp.Get_begin());
            std::move_backward(value_.Get_begin() + quantity, value_.Get_begin() + size_, tmp.Get_begin() + size_ + 1);
            tmp[quantity] = std::move(value);
            value_.swap(tmp);
            capacite_ = tmp_c;
        }
        ++size_;
        return &value_[quantity];
    }


    // "�������" ��������� ������� �������. ������ �� ������ ���� ������
    void PopBack() noexcept {
        if (value_) {
            --size_;
        }
    }

    // ������� ������� ������� � ��������� �������
    Iterator Erase(ConstIterator pos) {
        size_t quantity = pos - value_.Get_begin();
        std::move(value_.Get_begin() + quantity + 1, value_.Get_begin() + size_, value_.Get_begin() + quantity);
        --size_;
        return &value_[quantity];

    }

    // ���������� �������� � ������ ��������
    void swap(SimpleVector& other) noexcept {
        std::swap(other.size_, size_);
        std::swap(other.capacite_, capacite_);
        value_.swap(other.value_);

    }

private:
    ArrPtr<Type> value_;
    size_t size_ = 0;
    size_t capacite_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // ��������. �������� ���� ��������������
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // ��������. �������� ���� ��������������
    return !std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs < rhs || lhs == rhs;
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs > rhs || lhs == rhs;
}

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
};