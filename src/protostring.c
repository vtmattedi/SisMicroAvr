#include <protostring.h>

ProtoString::ProtoString()
{
}

const char* ProtoString::c_str()
{
    return data;
}

ProtoString::operator+=(char c)
{
    //this only works if we have all 0s in the data array
    if (length < MAX_LENGTH - 1)//-1 because we need to add the null terminator
    {
        this.data[length] = c;
        this.length++;
    }
    return *this;
}

ProtoString::operator=(char* c)
{
    
        this.length = 0;
        for (size_t i = 0; i < MAX_LENGTH; i++)
        {
           this.data[i] = 0;
        }
        
        while (*c != 0)
        {
            this.data[length] += *c;
            c++;
        }
    return *this;
}
