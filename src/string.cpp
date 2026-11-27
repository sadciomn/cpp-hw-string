#include "../string.hpp"
#include <iostream>

String::String() : data_(new char[1]{'\0'}) {}; // default constructor

String::String(
    const char* other) // constructor that takes c-string or std::string
{
    if (other == "")
    {
        return;
    }
    size_ = std::strlen(other);
    capacity_ = size_;
    data_ = new char[capacity_ + 1];
    std::strcpy(data_, other);
}

String::String(const size_t& n,
               const char& c) // constructor that takes number and char and then
                              // creates string with n chars
{
    size_ = n;
    capacity_ = n;
    data_ = new char[capacity_ + 1];
    for (size_t i = 0; i < size_; ++i)
    {
        data_[i] = c;
    }
    data_[size_] = '\0';
}

String::String(const String& other) // constructor that takes other object of
                                    // the same class String
{
    size_ = other.size_;
    capacity_ = other.size_;
    data_ = new char[capacity_ + 1];
    std::strcpy(data_, other.data_);
}

String::~String() { delete[] data_; }

size_t String::Size() const { return size_; }
size_t String::Length() const { return size_; } // same as size()
size_t String::Capacity() const
{
    return capacity_;
} // how much space does
  // string object has

char& String::operator[](size_t index) const { return data_[index]; }

std::ostream& operator<<(std::ostream& os, const String& str)
{
    for (size_t i = 0; i < str.Size(); ++i)
    {
        os << str[i];
    }
    return os;
}

void String::PushBack(char c)
{
    if (capacity_ > size_)
    {
        data_[size_] = c;
        ++size_;
        data_[size_] = '\0';
    }
}

// char* String::Data() const
// {
//     if (data_ == nullptr)
//     {
//         return
//     }
// };
