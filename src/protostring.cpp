#include <protostring.h>
#include <avr/io.h>
// Zero out the string and set the length to 0
void ProtoString::reset()
{
    for (int i = 0; i < MAX_LENGTH; i++)
    {
        data[i] = 0;
    }
    length = 0;
}

ProtoString &ProtoString::operator+=(const ProtoString &other)
{
    int newLength = length + other.length;
    if (newLength > MAX_LENGTH)
    {
        newLength = MAX_LENGTH;
    }

    for (int i = length; i < newLength; i++)
    {
        data[i] = other.data[i - length];
    }

    length = newLength;
    return *this;
}

ProtoString &ProtoString::operator+=(const char c)
{
    int newLength = length + 1;
    if (newLength > MAX_LENGTH)
    {
        newLength = MAX_LENGTH;
    }
    data[length] = c;

    length = newLength;
    return *this;
}

ProtoString &ProtoString::operator+=(const char* c)
{
    while (c != '\0')
    {
        *this += *c;
        c++;
    }
    
    return *this;
}

ProtoString &ProtoString::operator=(const ProtoString &other)
{
    reset();
    for (int i = 0; i < other.length; i++)
    {
        *this += other.data[i];
    }
    return *this;
}
ProtoString &ProtoString::operator=(const char* c)
{
    reset();
    while (c != '\0')
    {
        *this += *c;
        c++;
    }
    return *this;
}
bool ProtoString::operator==(const ProtoString &other)
{
    if (length != other.length)
    {
        return false;
    }
    for (int i = 0; i < length; i++)
    {
        if (data[i] != other.data[i])
        {
            return false;
        }
    }
    return true;
}
bool ProtoString::operator==(const char* c)
{
    int i = 0;
    while (c != '\0')
    {
        if (data[i] != *c)
        {
            return false;
        }
        i++;
        c++;
    }
    return true;
}

char ProtoString::operator[](int index)
{
    if (index < 0 || index >= length)
    {
        return 0;
    }
    return data[index];
}

const char* ProtoString::c_str()
{
    return data;
}

ProtoString ProtoString::substring(int start, int end)
{
    ProtoString str;
    if (end == 0)
    {
        end = length;
    }
    else if (end < 0)
    {
        end = length + end;
        if (end < start)
        {
            
        }
    }
    for (int i = start; i < end; i++)
    {
        str += data[i];
    }
    return str;
}