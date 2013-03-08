#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>

class PDBSourceInfo
{
public:
    struct FileInfo
    {
        const char *obj; size_t obj_len;
        const char *src; size_t src_len;
        const char *cwd; size_t cwd_len;
        const char *cl;  size_t cl_len;
        const char *cmd; size_t cmd_len;
    };
    std::vector<FileInfo> info;
    std::string pdb;

    bool load(const char *path);
};

bool MapFile(std::string &str, const char *path)
{
    if(FILE *f=fopen(path, "rb")) {
        fseek(f, 0, SEEK_END);
        str.resize(ftell(f));
        fseek(f, 0, SEEK_SET);
        fread(&str[0], 1, str.size(), f);
        fclose(f);
        return true;
    }
    return false;
}

bool PDBSourceInfo::load( const char *path )
{
    if(!MapFile(pdb, path) || pdb.empty()) { return false; }

    size_t goal = pdb.size()-4;
    for(size_t i=0; i<goal; ++i) {
        if( pdb[i+0]=='.' && pdb[i+1]=='o' && pdb[i+2]=='b' && pdb[i+3]=='j') {
            FileInfo tmp;
            const char *obj = &pdb[i];
            while(*obj!=':') { --obj; }
            tmp.obj = --obj;
            tmp.obj_len = strlen(tmp.obj);

            for(; i<goal; ++i) {
                if( pdb[i+0]=='c' && pdb[i+1]=='w' && pdb[i+2]=='d' && pdb[i+3]=='\0') {
                    tmp.cwd = &pdb[i+4];
                    tmp.cwd_len = strlen(tmp.cwd);
                    i += tmp.cwd_len+4;
                    break;
                }
            }

            for(; i<goal; ++i) {
                if( pdb[i+0]=='\0' && pdb[i+1]=='c' && pdb[i+2]=='l' && pdb[i+3]=='\0') {
                    tmp.cl = &pdb[i+4];
                    tmp.cl_len = strlen(tmp.cl);
                    i += tmp.cl_len+4;
                    break;
                }
            }

            for(; i<goal; ++i) {
                if( pdb[i+0]=='c' && pdb[i+1]=='m' && pdb[i+2]=='d' && pdb[i+3]=='\0') {
                    tmp.cmd = &pdb[i+4];
                    tmp.cmd_len = strlen(tmp.cmd);
                    i += tmp.cmd_len+4;
                    break;
                }
            }

            for(; i<goal; ++i) {
                if( pdb[i+0]=='s' && pdb[i+1]=='r' && pdb[i+2]=='c' && pdb[i+3]=='\0') {
                    tmp.src = &pdb[i+4];
                    tmp.src_len = strlen(tmp.src);
                    i += tmp.src_len+4;
                    break;
                }
            }

            if(strstr(tmp.obj, "f:\\dd\\vctools")!=NULL) { continue; }
            info.push_back(tmp);
        }
    }

    return true;
}

int main(int argc, char *argv[])
{
    PDBSourceInfo pdb;
    pdb.load("PDBSourceInfo.pdb");
    std::for_each(pdb.info.begin(), pdb.info.end(), [&](PDBSourceInfo::FileInfo &info){
        printf("obj: %s\n" "src: %s\n" "cwd: %s\n" "cl: %s\n" "cmd: %s\n\n", info.obj, info.src, info.cwd, info.cl, info.cmd);
    });
    return 0;
}

/*
$ cl PDBSourceInfo.cpp /Zi
$ ./PDBSourceInfo
obj: C:\project\scribble\PDBSourceInfo.obj
src: PDBSourceInfo.cpp
cwd: C:\project\scribble
cl: C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\BIN\cl.exe
cmd: -FoC:\project\scribble\PDBSourceInfo.obj -FdC:\project\scribble\vc100.pdb -Zi -MT -I"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\INCLUDE" -I"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\ATLMFC\INCLUDE" -I"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\include" -TP -X
*/