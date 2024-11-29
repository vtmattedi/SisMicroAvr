#define MAX_LENGTH 100
// Since we are using c, this is a struct to facilitate
// string manipulation
struct ProtoString
{
    char data[MAX_LENGTH + 1];
    int length;

    ProtoString();
    const char* c_str();

    ProtoString operator+=(char c);

    ProtoString operator=(char* c);

};
