add_subdirectory(bsd)

set(OST_IO_MSGPACK_MSGPACK_PREDEF_OS_HEADERS
irix.h
cygwin.h
linux.h
vms.h
ios.h
haiku.h
hpux.h
amigaos.h
android.h
unix.h
macos.h
solaris.h
windows.h
bsd.h
aix.h
os400.h
qnxnto.h
beos.h
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_PREDEF_OS_BSD_HEADERS
${OST_IO_MSGPACK_MSGPACK_PREDEF_OS_BSD_HEADERS}
PARENT_SCOPE
)

