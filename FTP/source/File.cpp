#include "../include/main.h"
#include "../include/File.h"

ssize_t read(int fd, void *buf, size_t nbyte)
{
    return syscall(3, fd, buf, nbyte);
}

ssize_t write(int fd, const void *buf, size_t count)
{
    return syscall(4, fd, buf, count);
}

int open(const char *path, int flags, int mode)
{
    return syscall(5, path, flags, mode);
}

int close(int fd)
{
    return syscall(6, fd);
}

int unlink(const char *pathname)
{
    return syscall(10, pathname);
}

int readlink(const char *path,	char *buf, int bufsiz)
{
    return syscall(58, path, buf, bufsiz);
}

int mount(const char *type, const char	*dir, int flags, void *data)
{
    return syscall(21, type, dir, flags, data);
}

int nmount(struct iovec *iov, uint32_t niov, int flags)
{
    return syscall(378, iov, niov, flags);
}

int unmount(const char *dir, int flags)
{
    return syscall(22, dir, flags);
}

int fchown(int fd, int uid, int gid)
{
    return syscall(123, fd, uid, gid);
}

int fchmod(int fd, int mode)
{
    return syscall(124, fd, mode);
}

int rename(const char *oldpath, const char *newpath)
{
    return syscall(128, oldpath, newpath);
}

int mkdir(const char *pathname, mode_t mode)
{
    return syscall(136, pathname, mode);
}

int rmdir(const char *path)
{
    return syscall(137, path);
}

int stat(const char *path, struct stat *sb)
{
    return syscall(188, path, sb);
}

int fstat(int fd, struct stat *sb)
{
    return syscall(189, fd, sb);
}

int fstatat(int fd, const char *path, struct stat *buf, int flag)
{
    return syscall(493, fd, path, buf, flag);
}

int getdents(int fd, char *buf, int count)
{
    return syscall(272, fd, buf, count);
}

off_t lseek(int fildes, off_t offset, int whence)
{
    return syscall(478, fildes, offset, whence);
}

int getSandboxDirectory(char *destination, int *length)
{
    return syscall(602, 0, destination, length);
}