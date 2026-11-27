#pragma once

#include <cstring>
#include <iostream>

class String
{
  private:
    char* data_;
    size_t size_ = 0;
    size_t capacity_ = 0;

  public:
    String();
    String(const char* other);
    String(const size_t& n, const char& c);
    String(const String& other);
    ~String();

    size_t Size() const;
    size_t Length() const;
    size_t Capacity() const;

    char& operator[](size_t index) const;
    String& operator=(const String& other)
    {
        if (this->capacity_ < other.capacity_)
        {
            size_ = other.size_;
            capacity_ = size_;
            delete[] data_;
            data_ = new char[other.capacity_ + 1];
            strcpy(data_, other.data_);
            return *this;
        }
        size_ = other.size_;
        strcpy(data_, other.data_);
    };

    void PushBack(char c) {};

    char* Data() const;
};