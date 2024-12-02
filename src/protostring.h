#pragma once
#define MAX_LENGTH 100

// Since we are using C, this is a struct to facilitate
// string manipulation
struct ProtoString
{
    char data[MAX_LENGTH + 1];
    int length;
};

// Function prototypes for string manipulation
void initProtoString(struct ProtoString* pstr);
const char* protoStringCStr(const struct ProtoString* pstr);
void protoStringAppendChar(struct ProtoString* pstr, char c);
void protoStringAssign(struct ProtoString* pstr, const char* c);