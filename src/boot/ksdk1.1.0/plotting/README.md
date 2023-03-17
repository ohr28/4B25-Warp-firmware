# 4B25 - Coursework #4

Harry Rodgers - ohr22
St John's College

## Displacement offset and error estimation

The project is contained in five files:

- A revised devMMA8451Q.c
- A revised devMMA8451Q.h
- A revised boot.c
- A new file accel_test.c
- A new file accel_test.h

These five files, along with this README.md are included in the submitted .zip archive.

The following files must also be amended however for the project to compile and successfully run:

Makefile:
    cp src/boot/ksdk1.1.0/accel_test.*				build/ksdk1.1/work/demos/Warp/src/

CMakeLists-Warp.txt:
    "${ProjDirPath}/../../src/accel_test.c"

These two files are not included in the submitted .zip archive.

accel_test.c and accel_test.h should be in the src/boot/ksdk1.1.0 directory along side boot.c.

