#!/bin/bash

./ispc --arch=x86-64 --addressing=64 --target=avx -h test.h --emit-obj -o out.obj --opt=fast-masked-vload --opt=fast-math --opt=force-aligned-memory test.ispc && \
dumpbin /disasm out.obj
