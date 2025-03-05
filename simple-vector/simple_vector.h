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

    explicit SimpleVector(size_t size) 
        : SimpleVector(size, Type{}) {
    }

    explicit SimpleVector(ReserveProxyObj new_capacity) 
        :  capacity_(new_capacity.capacity_to_reserve){
    }

    SimpleVector(size_t size, const Type& value) 
        : simple_vector_(size), size_(size), capacity_(size) {
        std::fill(begin(), end(), value);
    }

    SimpleVector(size_t size, const Type&& value) 
        : size_(size), capacity_(size) {
        ArrayPtr<Type> temp(size);
        for (auto it = temp.Get(); it != (temp.Get() + size); ++it) {
            *it = std::move(value);
        }
        temp.swap(simple_vector_);
    }

    SimpleVector(std::initializer_list<Type> init) 
        : simple_vector_(init.size()), size_(init.size()), capacity_(init.size()) {
        std::copy(std::make_move_iterator(init.begin()), std::make_move_iterator(init.end()), begin());
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

    void PushBack(const Type& item) {
        if (size_ >= capacity_) {
            size_t new_capacity = (capacity_ == 0 ? 1 : capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            if (new_capacity > 1) {
                std::copy(begin(), end(), temp.Get());
            }
            temp[size_] = item;
            simple_vector_.swap(temp);
            ++size_;
            capacity_ = new_capacity;
        }else {
            simple_vector_[size_] = item;
            ++size_;
        }
    }

    void PushBack(Type&& item) {
        if (size_ >= capacity_) {
            size_t new_capacity = (capacity_ == 0 ? 1 : capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            if (new_capacity > 1) {
                std::move(std::make_move_iterator(begin()), std::make_move_iterator(end()), temp.Get());
            }
            temp[size_] = std::move(item);
            simple_vector_.swap(temp);
            ++size_;
            capacity_ = new_capacity;
        }else {
            simple_vector_[size_] = std::move(item);
            ++size_;
        }
    }

    Iterator Insert(ConstIterator pos, const Type& value) {      
        size_t new_pos = static_cast<size_t>(std::distance(cbegin(), pos));
        if (size_ >= capacity_) {
            size_t new_capacity = (capacity_ == 0 ? 1 : capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            if (new_capacity > 1) {
                std::copy(begin(), begin() + new_pos, temp.Get());
                std::copy(begin() + new_pos, end(), temp.Get() + new_pos + 1);
            }
            temp[new_pos] = value;
            simple_vector_.swap(temp);
            ++size_;
            capacity_ = new_capacity;
        }else {
            std::copy(begin() + new_pos, end(), begin() + new_pos + 1);
            simple_vector_[new_pos] = value;
            ++size_;
        }
        return &simple_vector_[new_pos];               
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        size_t new_pos = static_cast<size_t>(std::distance(cbegin(), pos));
        if (size_ >= capacity_) {
            size_t new_capacity = (capacity_ == 0 ? 1 : capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            if (new_capacity > 1) {
                std::move(std::make_move_iterator(begin()), std::make_move_iterator(begin() + new_pos), temp.Get());
                std::move(std::make_move_iterator(begin() + new_pos), std::make_move_iterator(end()), temp.Get() + new_pos + 1);
            }
            temp[new_pos] = std::move(value);
            simple_vector_.swap(temp);
            ++size_;
            capacity_ = new_capacity;
        }else {
            std::move(std::make_move_iterator(begin() + new_pos), std::make_move_iterator(end()), begin() + new_pos + 1);
            simple_vector_[new_pos] = std::move(value);
            ++size_;
        }
        return &simple_vector_[new_pos];           
    }

    void PopBack() noexcept {
        if (!IsEmpty()) {
            size_--;
        }
    }

    Iterator Erase(ConstIterator pos) {
        if (size_ != 0) {
        auto temp_pos = pos - begin();
        std::move(std::make_move_iterator(begin() + temp_pos + 1), std::make_move_iterator(end()), begin() + temp_pos);
        size_--;
        return begin() + temp_pos;
        }
        return nullptr;
    
    }

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

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        return simple_vector_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return simple_vector_[index];
    }

    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("error");
        }
        return simple_vector_[index];
    }

    const Type& At(size_t index) const {
        if (index > size_ - 1) {
            throw std::out_of_range("error");
        }
        return simple_vector_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

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

    Iterator begin() noexcept {
        return simple_vector_.Get();
    }

    Iterator end() noexcept {
        return simple_vector_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return cbegin();
    }

    ConstIterator end() const noexcept {
        return cend();
    }

    ConstIterator cbegin() const noexcept {
        return simple_vector_.Get();
    }

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
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs <= rhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}               