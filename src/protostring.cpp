#include <protostring.h>
#include <avr/io.h>
// Zero out the string and set the length to 0
void initProtoString(struct ProtoString* pstr)
{    
    pstr->length = 0;
    for (int i = 0; i < MAX_LENGTH; i++)
    {
        pstr->data[i] = '\0';
    }

}
const char* protoStringCStr(const struct ProtoString* pstr)
{
    return pstr->data;
}
void protoStringAppendChar(struct ProtoString* pstr, char c)
{
    if (pstr->length < MAX_LENGTH)
    {
        pstr->data[pstr->length] = c;
        pstr->length++;
        pstr->data[pstr->length] = '\0';
    }
}
void protoStringAssign(struct ProtoString* pstr, const char* c)
{
    initProtoString(pstr);
    for (int i = 0; i < MAX_LENGTH; i++)
    {
        if (c[i] == '\0') // End of string
        {
            break;
        }
        protoStringAppendChar(pstr, c[i]);
    }

}

