#pragma once
#include "array_ptr.h"
#include <initializer_list>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <iterator>


//Не забудьте обновить указатель на начало вектора и текущую ёмкость.Для 
//реализации конструктора с резервированием вам понадобится 
//дополнительный класс - обёртка, чтобы компилятор мог разобраться и вызвать 
//правильный конструктор.Этот конструктор должен принимать по значению
//объект этого класса - обёртки.Тогда функция(не метод!) будет иметь следующую сигнатуру :
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

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) : SimpleVector(size, Type()) {
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) : value_(size), size_(size), capacite_(size) {
        std::fill(value_.Get_begin(), value_.Get_begin() + size, value);
    }

    // Создаёт вектор из std::initializer_list
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

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacite_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return value_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return value_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("out_of_range");
        }
        else {
            return value_[index];
        }
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
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

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return value_.Get_begin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return value_.Get_begin() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return value_.Get_begin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return value_.Get_begin() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return value_.Get_begin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
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

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
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

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
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


    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (value_) {
            --size_;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        size_t quantity = pos - value_.Get_begin();
        std::move(value_.Get_begin() + quantity + 1, value_.Get_begin() + size_, value_.Get_begin() + quantity);
        --size_;
        return &value_[quantity];

    }

    // Обменивает значение с другим вектором
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
    // Заглушка. Напишите тело самостоятельно
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
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