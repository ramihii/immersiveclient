set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_SYSROOT $ENV{TOOLCHAIN_HOME}/$ENV{TOOLDIR_ARM}/sysroot)

#set(CMAKE_SYSROOT /home/okka/code/crosscc/android/arm-api15/sysroot)

#set(AS arm-linux-androideabi-as)
#set(NM arm-linux-androideabi-nm)
#set(AR arm-linux-androideabi-ar)
#set(LD arm-linux-androideabi-ld)
#set(CC arm-linux-androideabi-gcc)
#set(CXX arm-linux-androideabi-g++)
#set(CPP arm-linux-androideabi-cpp)
#set(RANLIB arm-linux-androideabi-ranlib)

#set(CMAKE_LIBRRARY_PATH build/armeabi/lib)
#set(CMAKE_INCLUDE_PATH build/armeabi/include)
#set(CMAKE_AR arm-linux-androideabi-ar)
#set(CMAKE_AS arm-linux-androideabi-as)
#set(CMAKE_NM arm-linux-androideabi-nm)
#set(CMAKE_RANLIB arm-linux-androideabi-ranlib)
set(CMAKE_C_COMPILER arm-linux-androideabi-gcc)
set(CMAKE_CXX_COMPILER arm-linux-androideabi-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
