#include "string.hpp"
#include <cstring>
#include <algorithm>
#include <stdexcept>

// ==================== 构造 / 析构 ====================

string::string() : str(nullptr), len(0) {
    str = new char[1];
    str[0] = '\0';
}

string::string(const char* _str) {
    if (_str == nullptr) {
        str = new char[1];
        str[0] = '\0';
        len = 0;
    } else {
        len = std::strlen(_str);
        str = new char[len + 1];
        std::strcpy(str, _str);
    }
}

string::string(const string& other) {
    len = other.len;
    str = new char[len + 1];
    std::strcpy(str, other.str);
}

string::string(string&& other) noexcept : str(other.str), len(other.len) {
    other.str = nullptr;
    other.len = 0;
}

string::~string() {
    delete[] str;
}

// ==================== 赋值运算符 ====================

string& string::operator=(const string& other) {
    if (this != &other) {
        delete[] str;
        len = other.len;
        str = new char[len + 1];
        std::strcpy(str, other.str);
    }
    return *this;
}

string& string::operator=(string&& other) noexcept {
    if (this != &other) {
        delete[] str;
        str = other.str;
        len = other.len;
        other.str = nullptr;
        other.len = 0;
    }
    return *this;
}

string& string::operator=(const char* _str) {
    if (_str == nullptr) {
        delete[] str;
        len = 0;
        str = new char[1];
        str[0] = '\0';
    } else {
        const size_t new_len = std::strlen(_str);
        delete[] str;
        len = new_len;
        str = new char[len + 1];
        std::strcpy(str, _str);
    }
    return *this;
}

// ==================== 访问器 ====================

const char* string::c_str() const {
    return str ? str : "";
}

size_t string::size() const {
    return len;
}

size_t string::length() const {
    return len;
}

bool string::empty() const {
    return len == 0;
}

// ==================== 容量管理 ====================

void string::reserve(size_t new_cap) {
    if (new_cap > len) {
        char* new_str = new char[new_cap + 1];
        if (str) {
            std::strcpy(new_str, str);
        } else {
            new_str[0] = '\0';
        }
        delete[] str;
        str = new_str;
    }
}

size_t string::capacity() const {
    return len; // 简化实现：capacity == size
}

// ==================== 修改操作 ====================

void string::append(const char* _str) {
    if (_str == nullptr) return;
    size_t add_len = std::strlen(_str);
    char* new_str = new char[len + add_len + 1];
    if (str) {
        std::strcpy(new_str, str);
    } else {
        new_str[0] = '\0';
    }
    std::strcat(new_str, _str);
    delete[] str;
    str = new_str;
    len += add_len;
}

void string::append(const string& other) {
    append(other.str);
}

void string::push_back(char ch) {
    const char new_str[2] = { ch, '\0' };
    append(new_str);
}

void string::clear() {
    delete[] str;
    len = 0;
    str = new char[1];
    str[0] = '\0';
}

void string::swap(string& other) noexcept {
    std::swap(str, other.str);
    std::swap(len, other.len);
}

// ==================== 字符访问 ====================

char& string::operator[](size_t index) {
    return str[index];
}

const char& string::operator[](size_t index) const {
    return str[index];
}

char& string::at(size_t index) {
    if (index >= len) {
        throw std::out_of_range("string::at: index out of range");
    }
    return str[index];
}

const char& string::at(size_t index) const {
    if (index >= len) {
        throw std::out_of_range("string::at: index out of range");
    }
    return str[index];
}

// ==================== 拼接运算符 ====================

string string::operator+(const string& other) const {
    string result(*this);
    result.append(other);
    return result;
}

string string::operator+(const char* _str) const {
    string result(*this);
    result.append(_str);
    return result;
}

// ==================== 复合赋值运算符 ====================

string& string::operator+=(const string& other) {
    append(other);
    return *this;
}

string& string::operator+=(const char* _str) {
    append(_str);
    return *this;
}

string& string::operator+=(char ch) {
    push_back(ch);
    return *this;
}

// ==================== 比较运算符 ====================

bool string::operator==(const string& other) const {
    return std::strcmp(str, other.str) == 0;
}

bool string::operator==(const char* _str) const {
    if (_str == nullptr) {
        return len == 0;
    }
    return std::strcmp(str, _str) == 0;
}

bool string::operator!=(const string& other) const {
    return !(*this == other);
}

bool string::operator!=(const char* _str) const {
    return !(*this == _str);
}

bool string::operator<(const string& other) const {
    return std::strcmp(str, other.str) < 0;
}

bool string::operator>(const string& other) const {
    return std::strcmp(str, other.str) > 0;
}

// ==================== 友元函数 ====================

string operator+(const char* lhs, const string& rhs) {
    string result(lhs);
    result.append(rhs);
    return result;
}

bool operator==(const char* lhs, const string& rhs) {
    return rhs == lhs;
}

bool operator!=(const char* lhs, const string& rhs) {
    return rhs != lhs;
}

// ==================== 流运算符 ====================

std::ostream& operator<<(std::ostream& os, const string& str) {
    os << str.str;
    return os;
}

std::istream& operator>>(std::istream& is, string& str) {
    char buffer[1024];
    if (is >> buffer) {
        str = buffer;
    }
    return is;
}
