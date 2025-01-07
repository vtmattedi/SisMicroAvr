#pragma once
#define MAX_LENGTH 100

// Since we did c string manipulation, on first problem, this time we will use
// c++ classes and overloads to handle strings.
// Not gonna implement dynamic memory allocation, so the string will have a fixed size.
class ProtoString
{
private:
    // Zero out the data vector and set the length to 0
    void reset();

public:
    // Make sure to initialize the string to 0
    char data[MAX_LENGTH] = {0};
    // Current length of the string
    int length = 0;

    // Every operator I should need:
    // += char*
    ProtoString &ProtoString::operator+=(const char *c);
    // += char
    ProtoString &ProtoString::operator+=(const char c);
    // += string
    ProtoString &ProtoString::operator+=(const ProtoString &other);
    // = string
    ProtoString &ProtoString::operator=(const ProtoString &other);
    // = char*
    ProtoString &ProtoString::operator=(const char *c);
    // == string
    bool ProtoString::operator==(const ProtoString &other);
    // == char*
    bool ProtoString::operator==(const char *c);
    // access string at index i; returns 0 if out of bounds.
    // The array is always initialized to 0, so it should be safe.
    char ProtoString::operator[](int index);
    const char *c_str();
    ProtoString substring(int start, int end = 0);
};