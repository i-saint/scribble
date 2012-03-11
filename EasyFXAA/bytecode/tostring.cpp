#include <string>
#include <cstdio>

int main(int argc, char *argv[])
{
    if(argc!=3) {
        printf("usage: %s [in file] [symbol name]\n", argv[0]);
        return 0;
    }

    std::string data;
    std::string hex;
    if(FILE *fin = fopen(argv[1], "rb")) {
        fseek(fin, 0, SEEK_END);
        data.resize(ftell(fin));
        fseek(fin, 0, SEEK_SET);
        fread(&data[0], 1, data.size(), fin);
        fclose(fin);
    }
    else {
        printf("%s could not open.\n", argv[1]);
        return 1;
    }

    hex.reserve(data.size()*4+128);
    {
        char buf[128];
        sprintf(buf, "const size_t %s_size=%d;\n", argv[2], data.size());
        hex += buf;
        sprintf(buf, "const char %s[]=\"", argv[2]);
        hex += buf;
    }
    for(std::string::iterator i=data.begin(); i!=data.end(); ++i) {
        char buf[32];
        sprintf(buf, "\\x%02x", (unsigned char)*i);
        hex += buf;
    }
    hex += "\";\n";

    fwrite(hex.c_str(), 1, hex.size(), stdout);
}
