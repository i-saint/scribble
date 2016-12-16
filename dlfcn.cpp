#include <cstdio>
#include <dlfcn.h>
#include <link.h>

int main(int argc, char *argv[])
{
    Dl_info info;
    link_map *lmap;
    int i = dladdr1((void*)&main, &info, (void**)&lmap, RTLD_DL_LINKMAP);
    printf("%s, %s\n", info.dli_fname, lmap->l_name);
}
