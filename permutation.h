//
// Created by gosktin on 22.09.16.
//

#ifndef PERMUTATION_H
#define PERMUTATION_H

#include <iostream>

template <typename T>
void swap(T *left, T *right) {
    if (left != right) {
        T *temp = new T(*left);

        *left = *right;
        *right = *temp;

        delete temp;
    }
}

template <typename T>
void reverse(T *start, T *stop) {
    while (start < stop)
        swap(start++, stop--);
}

template <typename T>
size_t mergeWithCount(const T *first, const T *const firstEnd,
                      const T *second, const T *const secondEnd, T *buffer) {
    size_t count = 0;
    const T *leftElementsCount = firstEnd;

    while (first < firstEnd || second < secondEnd)
        if (first == firstEnd)
            *buffer++ = *second++;
        else if (second == secondEnd)
            *buffer++ = *first++;
        else if (*first < *second)
            *buffer++ = *first++;
        else {
            count += leftElementsCount - first;
            *buffer++ = *second++;
        }
    return count;
}

template <typename T>
size_t mergeSortWithCount(T *start, T *finish) {
    if (start < finish - 1) {
        T middle = (finish - start) / 2;

        size_t count1 = mergeSortWithCount(start, start + middle);
        size_t count2 = mergeSortWithCount(start + middle, finish);

        T *buffer = new T[finish - start];
        size_t count3 = mergeWithCount(start, start + middle, start + middle, finish, buffer);

        T *iter = buffer;
        while (start < finish)
            *start++ = *iter++;

        delete[] buffer;

        return count1 + count2 + count3;
    }
    return 0;
}

class Permutation {
public:
    explicit Permutation(unsigned int n);
    Permutation(const Permutation &object);

    template <typename T>
    explicit Permutation(unsigned int n, const T *const raw);

    ~Permutation();

    size_t derangementsCount() const;

    bool isOdd() const;
    bool isEven() const;

    const Permutation pow(int degree) const;

    Permutation &operator*=(const Permutation &right);
    const Permutation operator*(const Permutation &right) const;
    Permutation &operator=(const Permutation &right);

    const Permutation next() const;
    Permutation &operator++();
    const Permutation operator++(int);
    const Permutation previous() const;
    Permutation &operator--();
    const Permutation operator--(int);

    const Permutation inverse() const;

    bool operator==(const Permutation &right) const;
    bool operator!=(const Permutation &right) const;
    bool operator<(const Permutation &right) const;
    bool operator>(const Permutation &right) const;
    bool operator<=(const Permutation &right) const;
    bool operator>=(const Permutation &right) const;

    unsigned int operator[](unsigned int index) const;

    template <typename T>
    void operator()(T *object);

    size_t getElementsCount() const;

private:
    unsigned int size_;
    unsigned int *data_;

    enum modeType_ {
        MT_next,
        MT_previous
    };

    const Permutation getNextOrPreviousPermutation(const modeType_ m) const;
};

Permutation::Permutation(unsigned int n) : size_(n) {
    data_ = new unsigned int[size_];
    for (unsigned int iter = 0; iter < size_; ++iter)
        data_[iter] = iter;
}

template <typename T>
Permutation::Permutation(unsigned int n, const T *const raw) : size_(n) {
    data_ = new unsigned int[size_];
    for (unsigned int iter = 0; iter < size_; ++iter)
        data_[iter] = raw[iter];
}

Permutation::Permutation(const Permutation &object) : size_(object.size_) {
    if (this == &object)
        return;
    data_ = new unsigned int[size_];
    for (unsigned int iter = 0; iter < size_; ++iter)
        data_[iter] = object[iter];
}

Permutation::~Permutation() {
    delete[] data_;
}

size_t Permutation::derangementsCount() const {
    Permutation temp(*this);

    return mergeSortWithCount(temp.data_, temp.data_ + temp.size_);
}

bool Permutation::isEven() const {
    return !isOdd();
}

const Permutation Permutation::pow(int degree) const {
    Permutation temp(*this);
    unsigned int constant = static_cast<unsigned int>(-1);

    for (unsigned int i = 0; i < size_; ++i)
        temp.data_[i] = constant;

    for (unsigned int i = 0; i < size_; ++i) {
        if (temp.data_[i] != constant)
            continue;

        int count = 1;
        for (unsigned int j = data_[i]; j != i; j = data_[j])
            ++count;
        count = (degree + count - 1) % count;

        unsigned int two = i;
        for (; count; --count)
            two = data_[two];

        for(unsigned int one = i; temp.data_[one] == constant; one = data_[one], two = data_[two])
            temp.data_[one] = data_[two];
    }
    return temp;
}

bool Permutation::isOdd() const {
    bool *used = new bool[size_];
    for (unsigned int i = 0; i < size_; ++i)
        used[i] = false;

    bool res = false;

    for (unsigned int i = 0; i < size_; ++i)
        if (!used[i]) {
            used[i] = true;
            unsigned int j = data_[i];
            while (j != i) {
                used[j] = true;
                res ^= 1;
                j = data_[j];
            }
        }

    delete[] used;
    return res;
}

Permutation &Permutation::operator*=(const Permutation &right) {
    if (this == &right or size_ != right.size_)
        return *this;
    else {
        Permutation *temp = new Permutation(*this);

        for (unsigned int i = 0; i < size_; ++i)
            data_[i] = right[temp->data_[i]];

        delete temp;
    }

    return *this;
}

const Permutation Permutation::operator*(const Permutation &right) const {
    Permutation temp(right);

    return temp *= *this;
}

Permutation &Permutation::operator=(const Permutation &right) {
    if (this == &right)
        return *this;

    if (right.size_ != size_) {
        delete[] data_;

        size_ = right.size_;
        data_ = new unsigned int[size_];
    }

    for (unsigned int i = 0; i < size_; ++i)
        data_[i] = right.data_[i];

    return *this;
}

const Permutation Permutation::getNextOrPreviousPermutation(const modeType_ m) const {
    if (size_ < 2)
        return *this;

    Permutation object(*this);

    long long element;
    for (long long iter = object.size_ - 2; iter > -1; --iter)
        if ((m == MT_next && object.data_[iter] < object.data_[iter + 1]) ||
            (m == MT_previous && object.data_[iter] > object.data_[iter + 1])) {
            element = iter + 1;
            for (long long j = element; j < object.size_; ++j)
                if (m == MT_next) {
                    if (object.data_[j] < object.data_[element] && object.data_[j] > object.data_[iter])
                        element = j;
                } else if (object.data_[j] > object.data_[element] && object.data_[j] < object.data_[iter])
                    element = j;

            swap(&object.data_[iter], &object.data_[element]);
            reverse(object.data_ + iter + 1, object.data_ + object.size_ - 1);

            break;
        }

    return object;
}

const Permutation Permutation::next() const {
    return getNextOrPreviousPermutation(MT_next);
}

Permutation &Permutation::operator++() {
    *this = next();
    return *this;
}

const Permutation Permutation::operator++(int) {
    Permutation temp(*this);
    *this = next();

    return temp;
}

const Permutation Permutation::previous() const {
    return getNextOrPreviousPermutation(MT_previous);
}

Permutation &Permutation::operator--() {
    *this = previous();

    return *this;
}

const Permutation Permutation::operator--(int) {
    Permutation temp(*this);
    *this = previous();

    return temp;
}

const Permutation Permutation::inverse() const {
    if (size_ < 2)
        return *this;

    Permutation temp(*this);

    for (unsigned int i = 0; i < size_; ++i)
        temp.data_[data_[i]] = i;

    return temp;
}

bool Permutation::operator==(const Permutation &right) const {
    if (size_ != right.size_)
        return false;

    for (unsigned int i = 0; i < size_; ++i)
        if (data_[i] != right.data_[i])
            return false;

    return true;
}

bool Permutation::operator!=(const Permutation &right) const {
    return !(*this == right);
}

bool Permutation::operator<(const Permutation &right) const {
    if (size_ < right.size_)
        return true;

    for (unsigned int i = 0; i < size_; ++i)
        if (data_[i] < right[i])
            return true;
        else if (data_[i] > right[i])
            return false;

    return false;
}

bool Permutation::operator>(const Permutation &right) const {
    return right < *this;
}

bool Permutation::operator<=(const Permutation &right) const {
    return !(*this > right);
}

bool Permutation::operator>=(const Permutation &right) const {
    return !(*this < right);
}

unsigned int Permutation::operator[](unsigned int index) const {
    return data_[index];
}

template <typename T>
void Permutation::operator()(T *object) {
    T *cobject = new T[size_];
    for (unsigned int i = 0; i < size_; ++i)
        cobject[i] = object[i];

    for (unsigned int i = 0; i < size_; ++i)
        object[data_[i]] = cobject[i];

    delete[] cobject;
}

std::ostream& operator<<(std::ostream &out, const Permutation &object) {
    out.flush();
    for (unsigned int i = 0; i < object.getElementsCount(); ++i)
        out << object[i] << ' ';
    return out;
}

size_t Permutation::getElementsCount() const {
    return static_cast<size_t>(size_);
}

#endif //PERMUTATION_H