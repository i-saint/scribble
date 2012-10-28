#include <cstdio>
#include <string>
#include <vector>
#include <regex>





#define USG_Generate
#define USG_SetSerializerGenerator      GenerateCode
#define USG_SetDeserializerGenerator    GenerateCode
#include "USG.h"



class Serializer
{
public:
    template<class T> Serializer& operator&(const T &v);
};

class Deserializer
{
public:
    template<class T> Deserializer& operator&(T &v);
};

template<> Serializer& Serializer::operator&<int>(const int &v) { return *this; }



class Hoge
{
public:
    int a;
    int b;
};

class Hage : public Hoge
{
public:
    int c;
    int d;

    void serialize(Serializer &s);
    void deserialize(Deserializer &s);
};

inline std::string GenSerializer(const char *member_name, const int flags)
{
    char buf[1024];
    sprintf(buf, "    s << %s;\n", member_name);
    return buf;
}

inline std::string GenDeserializer(const char *member_name, const int flags)
{
    char buf[1024];
    sprintf(buf, "    s >> %s;\n", member_name);
    return buf;
}

USG_GenerateBlockBegin(Hage)

void Hage::serialize(Serializer &s)
{
USG_SerializeBlockBegin(s)
USG_SerializeBlockEnd()
}

void Hage::deserialize(Deserializer &s)
{
USG_DeserializeBlockBegin(s)
USG_DeserializeBlockEnd()
}

USG_GenerateBlockEnd()


int main(int argc, char* argv[])
{
    USG_ExecGenerate;

    return 0;
}

/*
> cl /EHsc /Zi EnumMemberVariables.cpp
> EnumMemberVariables

Hoge::a
Hoge::b
Hage::c
Hage::d
*/
