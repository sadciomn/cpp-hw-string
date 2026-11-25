#include "../string.hpp"
#include <cstring>
#include <iostream>

String::String(const char* other)
{
    if (other == "")
    {
        return;
    }
    size_ = std::strlen(other);
    data_ = new char[size_ + 1];
    strcpy(data_, other);
}

String::String(const size_t& n, const char& c)
{
    size_ = n;
    data_ = new char[size_ + 1];
    for (size_t i = 0; i < size_; ++i)
    {
        data_[i] = c;
    }
    data_[size_] = '\0';
}

String::~String() { delete[] data_; }

size_t String::size() const { return size_; }

char& String::operator[](size_t index) const { return data_[index]; }

std::ostream& operator<<(std::ostream& os, const String& str)
{
    for (size_t i = 0; i < str.size(); ++i)
    {
        os << str[i];
    }
    return os;
}
