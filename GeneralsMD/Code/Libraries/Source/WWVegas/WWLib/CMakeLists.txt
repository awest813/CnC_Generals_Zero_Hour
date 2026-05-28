# WWLib – Westwood core utility library (portable subset) – Zero Hour tree.

set(WWLIB_MD_PORTABLE_SOURCES
    argv.cpp
    b64pipe.cpp
    b64straw.cpp
    base64.cpp
    blowfish.cpp
    buff.cpp
    chunkio.cpp
    crc.cpp
    cstraw.cpp
    gcd_lcm.cpp
    hash.cpp
    ini.cpp
    int.cpp
    lcw.cpp
    load.cpp
    mpmath.cpp
    multilist.cpp
    nstrdup.cpp
    pipe.cpp
    pk.cpp
    random.cpp
    rc4.cpp
    readline.cpp
    realcrc.cpp
    rle.cpp
    rndstrng.cpp
    sha.cpp
    slnode.cpp
    srandom.cpp
    straw.cpp
    tagblock.cpp
    textfile.cpp
    trim.cpp
    vector.cpp
    wwfile.cpp
    wwstring.cpp
    xpipe.cpp
    xstraw.cpp
)

# Win32-only sources — included only when targeting Windows.
set(WWLIB_MD_WIN32_SOURCES
    _convert.cpp
    _mono.cpp
    _timer.cpp
    blit.cpp
    bufffile.cpp
    convert.cpp
    cpudetect.cpp
    data.cpp
    ddraw.cpp
    dsurface.cpp
    ffactory.cpp
    hsv.cpp
    jshell.cpp
    keyboard.cpp
    LaunchWeb.cpp
    mixfile.cpp
    mono.cpp
    msgloop.cpp
    mpu.cpp
    mutex.cpp
    palette.cpp
    pcx.cpp
    ramfile.cpp
    rawfile.cpp
    rcfile.cpp
    refcount.cpp
    registry.cpp
    rgb.cpp
    sampler.cpp
    stimer.cpp
    strtok_r.cpp
    systimer.cpp
    thread.cpp
    verchk.cpp
    win.cpp
    wwfont.cpp
    WWCOMUtil.cpp
    xsurface.cpp
)

set(WWLIB_MD_SOURCES ${WWLIB_MD_PORTABLE_SOURCES})

if(WIN32)
    list(APPEND WWLIB_MD_SOURCES ${WWLIB_MD_WIN32_SOURCES})
endif()

# Prefix all sources with the current directory
list(TRANSFORM WWLIB_MD_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

file(GLOB WWLIB_MD_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(WWLibMD STATIC ${WWLIB_MD_SOURCES} ${WWLIB_MD_HEADERS})

target_include_directories(WWLibMD PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GENERALSMD_INCLUDE_DIRS}
)
