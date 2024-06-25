#pragma once

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <iterator>

#include "array_ptr.h"

struct ReserveProxyObj {
    ReserveProxyObj(size_t new_capacity) 
        : capacity_to_reserve(new_capacity) {
    }
    size_t capacity_to_reserve = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) 
        : SimpleVector(size, Type{}) {
    }

    explicit SimpleVector(ReserveProxyObj new_capacity) 
        :  capacity_(new_capacity.capacity_to_reserve){
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) 
        : size_(size), capacity_(size) {
        ArrayPtr<Type> temp(size);
        std::fill(temp.Get(), temp.Get() + size, value);
        temp.swap(simple_vector_);
    }

    SimpleVector(size_t size, const Type&& value) 
        : size_(size), capacity_(size) {
        ArrayPtr<Type> temp(size);
        for (auto it = temp.Get(); it != (temp.Get() + size); ++it) {
            *it = std::move(value);
        }
        temp.swap(simple_vector_);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) 
        : size_(init.size()), capacity_(init.size()) {
        ArrayPtr<Type> temp(init.size());
        std::move(std::make_move_iterator(init.begin()), std::make_move_iterator(init.end()), temp.Get());
        temp.swap(simple_vector_);
    }

    SimpleVector(const SimpleVector& other) 
        :simple_vector_(other.size_), size_(other.size_), capacity_(other.capacity_) {
        std::copy(other.begin(), other.end(), begin());
    }

    SimpleVector(SimpleVector&& other) 
        : simple_vector_(other.size_) {
        std::move(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()), begin());
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector temp(rhs.size_);
            std::copy(rhs.begin(), rhs.end(), temp.begin());
            swap(temp);
        }
        return *this;
    } 

    SimpleVector& operator=(SimpleVector&& rhs) {
        if (this != &rhs) {
            SimpleVector temp(rhs.size_);
            std::copy(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()), temp.begin());
            swap(temp);
        }
        return *this;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if (capacity_ == 0) {
            ArrayPtr<Type> temp(1);
            temp[0] = item;
            temp.swap(simple_vector_);
            ++size_;
            ++capacity_;
        }
        else if (size_ < capacity_) {
            simple_vector_[size_] = item;
            ++size_;
        }else {
            ArrayPtr<Type> temp(capacity_ * 2);
            std::copy(begin(), end(), temp.Get());
            temp[size_] = item;
            simple_vector_.swap(temp);
            ++size_;
            capacity_ *= 2;
        }
    }

    void PushBack(Type&& item) {
        if (capacity_ == 0) {
            ArrayPtr<Type> temp(1);
            temp[0] = std::move(item);
            temp.swap(simple_vector_);
            ++size_;
            ++capacity_;
        }
        else if (size_ < capacity_) {
            simple_vector_[size_] = std::move(item);
            ++size_;
        }else {
            ArrayPtr<Type> temp(capacity_ * 2);
            std::move(std::make_move_iterator(begin()), std::make_move_iterator(end()), temp.Get());
            temp[size_] = std::move(item);
            simple_vector_.swap(temp);
            ++size_;
            capacity_ *= 2;
        }
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        size_t new_pos = static_cast<size_t>(std::distance(cbegin(), pos));
        if (size_ < capacity_) {
            std::copy(begin() + new_pos, end(), begin() + new_pos + 1);
            simple_vector_[new_pos] = value;
            ++size_;
            return &simple_vector_[new_pos];
        }
        else if (size_ == 0 && capacity_ == 0) {
            ArrayPtr<Type> temp(1);
            temp[0] = value;
            simple_vector_.swap(temp);
            ++size_;
            ++capacity_;
            return &simple_vector_[0];
        }
        else {
            ArrayPtr<Type> temp(capacity_ * 2);
            std::copy(begin(), begin() + new_pos, temp.Get());
            std::copy(begin() + new_pos, end(), temp.Get() + new_pos + 1);
            temp[new_pos] = value;
            simple_vector_.swap(temp);
            ++size_;
            capacity_ *= 2;
            return &simple_vector_[new_pos];
        }
        
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        size_t new_pos = static_cast<size_t>(std::distance(cbegin(), pos));
        if (size_ < capacity_) {
            std::move(std::make_move_iterator(begin() + new_pos), std::make_move_iterator(end()), begin() + new_pos + 1);
            simple_vector_[new_pos] = std::move(value);
            ++size_;
            return &simple_vector_[new_pos];
        }
        else if (size_ == 0 && capacity_ == 0) {
            ArrayPtr<Type> temp(1);
            temp[0] = std::move(value);
            simple_vector_.swap(temp);
            ++size_;
            ++capacity_;
            return &simple_vector_[0];
        }
        else {
            ArrayPtr<Type> temp(capacity_ * 2);
            std::move(std::make_move_iterator(begin()), std::make_move_iterator(begin() + new_pos), temp.Get());
            std::move(std::make_move_iterator(begin() + new_pos), std::make_move_iterator(end()), temp.Get() + new_pos + 1);
            temp[new_pos] = std::move(value);
            simple_vector_.swap(temp);
            ++size_;
            capacity_ *= 2;
            return &simple_vector_[new_pos];
        }
        
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (!IsEmpty()) {
            size_--;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        if (size_ != 0) {
        auto temp_pos = pos - begin();
        std::move(std::make_move_iterator(begin() + temp_pos + 1), std::make_move_iterator(end()), begin() + temp_pos);
        size_--;
        return begin() + temp_pos;
        }
        return nullptr;
    
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        simple_vector_.swap(other.simple_vector_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    void swap(SimpleVector&& other) noexcept {
        simple_vector_.swap(other.simple_vector_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return simple_vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return simple_vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("error");
        }
        return simple_vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index > size_ - 1) {
            throw std::out_of_range("error");
        }
        return simple_vector_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (size_ >= new_size) {
            size_ = new_size;
        }
        else if (capacity_ >= new_size) {
            for (auto it = simple_vector_.Get() + size_;  it != simple_vector_.Get() + new_size; ++it) {
                *it = Type{};
            }
            size_ = new_size;
        }else {
            ArrayPtr<Type> temp(new_size);
            std::move(std::make_move_iterator(simple_vector_.Get()), std::make_move_iterator(simple_vector_.Get() + size_), temp.Get());
            for (auto it = temp.Get() + size_; it != temp.Get() + new_size; ++it) {
                *it = Type{};
            }
            size_ = new_size;
            capacity_ = new_size;
            temp.swap(simple_vector_);
        }
    }

    void Reserve(size_t new_capacity) { 
        if (new_capacity > capacity_) {
            ArrayPtr<Type> temp(new_capacity);
            if (size_ != 0) {
                std::copy(begin(), end(), temp.Get());
            }
            simple_vector_.swap(temp);
            capacity_ = new_capacity;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return simple_vector_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return simple_vector_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return cbegin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return cend();
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return simple_vector_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return simple_vector_.Get() + size_;
    }
private:
    ArrayPtr<Type> simple_vector_;
    size_t size_ = 0;
    size_t capacity_ = 0;
    
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if ((lhs < rhs) || (lhs == rhs)) {
        return true;
    }
        return false;
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if ((lhs > rhs) || (lhs == rhs)) {
        return true;
    }
        return false;
} 