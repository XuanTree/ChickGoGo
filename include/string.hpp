#ifndef STRING_HPP
#define STRING_HPP

#pragma once
#include <istream>

class string {
private:
    char* str;
    size_t len;
public:
    // 构造函数
    string();
    explicit string(const char* _str);
    string(const string& other);
    string(string&& other) noexcept;

    // 析构函数
    ~string();

    string& operator=(const string& other);
    string& operator=(string&& other) noexcept;
    string& operator=(const char* _str);

    // 访问器
    [[nodiscard]] const char* c_str() const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] size_t length() const;
    [[nodiscard]] bool empty() const;

    // 容量管理
    void reserve(size_t new_cap);
    [[nodiscard]] size_t capacity() const;

    // 修改操作
    void append(const char* _str);
    void append(const string& other);
    void push_back(char ch);
    void clear();
    void swap(string& other) noexcept;

    // 字符访问
    char& operator[](size_t index);
    const char& operator[](size_t index) const;
    char& at(size_t index);
    [[nodiscard]] const char& at(size_t index) const;

    // 运算符重载
    string operator+(const string& other) const;
    string operator+(const char* _str) const;

    string& operator+=(const string& other);
    string& operator+=(const char* _str);
    string& operator+=(char ch);

    bool operator==(const string& other) const;
    bool operator==(const char* _str) const;
    bool operator!=(const string& other) const;
    bool operator!=(const char* _str) const;
    bool operator<(const string& other) const;
    bool operator>(const string& other) const;

    friend string operator+(const char* lhs, const string& rhs);
    friend bool operator==(const char* lhs, const string& rhs);
    friend bool operator!=(const char* lhs, const string& rhs);

    friend std::ostream& operator<<(std::ostream& os, const string& str);
    friend std::istream& operator>>(std::istream& is, string& str);
};

#endif // STRING_HPP
