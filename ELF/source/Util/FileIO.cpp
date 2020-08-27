#include "../Main.hpp"
#include "FileIO.hpp"

extern "C"
{
	#include "sys/sysproto.h"
}

int FileIO_errorno = 0;

ssize_t sys_fread(int fd, void *buf, size_t nbyte)
{
	struct sys_read_args {
		uint64_t fd;
		uint64_t buf;
		uint64_t nbyte;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_read = (int(*)(thread * td, sys_read_args * uap))sysents[3].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_read_args uap;
	uap.fd = fd;
	uap.buf = (uint64_t)buf;
	uap.nbyte = nbyte;

	FileIO_errorno = sys_read(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (ssize_t)td->td_retval[0];
}

ssize_t sys_fwrite(int fd, const void *buf, size_t count)
{
	struct sys_write_args {
		uint64_t fd;
		uint64_t buf;
		uint64_t count;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_write = (int(*)(thread * td, sys_write_args * uap))sysents[4].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_write_args uap;
	uap.fd = fd;
	uap.buf = (uint64_t)buf;
	uap.count = count;

	FileIO_errorno = sys_write(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (ssize_t)td->td_retval[0];
}

int sys_fopen(char *path, int flags, int mode)
{
    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_open = (int(*)(thread * td, open_args * uap))sysents[5].sy_call;

	thread *td = curthread();

	int error;
    td->td_retval[0] = 0;

	open_args uap;
    uap.path = path;
	uap.flags = flags;
	uap.mode = mode;

	error = sys_open(td, &uap);
	if(error)
		return -error;

	return (int)td->td_retval[0];
}

int sys_fclose(int fd)
{
	struct sys_close_args {
		uint64_t fd;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_close = (int(*)(thread * td, sys_close_args * uap))sysents[6].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_close_args uap;
	uap.fd = fd;

	FileIO_errorno = sys_close(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_unlink(const char *pathname)
{
	struct sys_unlink_args {
		uint64_t pathname;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_unlink = (int(*)(thread * td, sys_unlink_args * uap))sysents[10].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_unlink_args uap;
	uap.pathname = (uint64_t)pathname;

	FileIO_errorno = sys_unlink(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_readlink(const char *path,	char *buf, int buflen)
{
	struct sys_readlink_args {
		uint64_t path;
		uint64_t buf;
		uint64_t buflen;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_readlink = (int(*)(thread * td, sys_readlink_args * uap))sysents[58].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_readlink_args uap;
    uap.path = (uint64_t)path;
    uap.buf = (uint64_t)buf;
	uap.buflen = buflen;

	FileIO_errorno = sys_readlink(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_mount(const char *type, const char	*dir, int flags, void *data)
{
	struct sys_mount_args {
		uint64_t type;
		uint64_t dir;
		uint64_t flags;
        uint64_t data;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_mount = (int(*)(thread * td, sys_mount_args * uap))sysents[21].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_mount_args uap;
	uap.type = (uint64_t)type;
	uap.dir = (uint64_t)dir;
	uap.flags = flags;
    uap.data = (uint64_t)data;

	FileIO_errorno = sys_mount(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_nmount(struct iovec *iov, uint32_t niov, int flags)
{
	struct sys_nmount_args {
		uint64_t iov;
		uint64_t niov;
		uint64_t flags;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_nmount = (int(*)(thread * td, sys_nmount_args * uap))sysents[378].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_nmount_args uap;
	uap.iov = (uint64_t)iov;
	uap.niov = niov;
	uap.flags = flags;

	FileIO_errorno = sys_nmount(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_unmount(const char *dir, int flags)
{
	struct sys_unmount_args {
		uint64_t dir;
		uint64_t flags;
		uint64_t nbyte;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_unmount = (int(*)(thread * td, sys_unmount_args * uap))sysents[22].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_unmount_args uap;
	uap.dir = (uint64_t)dir;
	uap.flags = flags;

	FileIO_errorno = sys_unmount(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_fchown(int fd, int uid, int gid)
{
	struct sys_fchown_args {
		uint64_t fd;
		uint64_t uid;
		uint64_t gid;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_fchown = (int(*)(thread * td, sys_fchown_args * uap))sysents[123].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_fchown_args uap;
	uap.fd = fd;
	uap.uid = uid;
	uap.gid = gid;

	FileIO_errorno = sys_fchown(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_fchmod(int fd, int mode)
{
	struct sys_fchmod_args {
		uint64_t fd;
		uint64_t mode;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_fchmod = (int(*)(thread * td, sys_fchmod_args * uap))sysents[124].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_fchmod_args uap;
	uap.fd = fd;
	uap.mode = mode;

	FileIO_errorno = sys_fchmod(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_rename(const char *oldpath, const char *newpath)
{
	struct sys_rename_args {
		uint64_t oldpath;
		uint64_t newpath;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_rename = (int(*)(thread * td, sys_rename_args * uap))sysents[128].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_rename_args uap;
	uap.oldpath = (uint64_t)oldpath;
	uap.newpath = (uint64_t)newpath;

	FileIO_errorno = sys_rename(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_mkdir(char *pathname, int mode)
{
	struct sys_mkdir_args {
		uint64_t pathname;
		uint64_t mode;
	};

    auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_mkdir = (int(*)(thread * td, mkdir_args * uap))sysents[136].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	mkdir_args uap;
	uap.path = pathname;
	uap.mode = mode;

	FileIO_errorno = sys_mkdir(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_rmdir(const char *path)
{
	struct sys_rmdir_args {
		uint64_t path;
		uint64_t buf;
		uint64_t nbyte;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_rmdir = (int(*)(thread * td, sys_rmdir_args * uap))sysents[137].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_rmdir_args uap;
	uap.path = (uint64_t)path;

	FileIO_errorno = sys_rmdir(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_stat(char *path, struct stat *sb) 
{
	struct sys_stat_args {
		uint64_t path;
		uint64_t sb;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_stat = (int(*)(thread * td, stat_args * uap))sysents[188].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	stat_args uap;
	uap.path = path;
	uap.ub = sb;

	FileIO_errorno = sys_stat(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_fstat(int fd, struct stat *sb)
{
	struct sys_fstat_args {
		uint64_t fd;
		uint64_t sb;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_fstat = (int(*)(thread * td, sys_fstat_args * uap))sysents[189].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_fstat_args uap;
	uap.fd = fd;
	uap.sb = (uint64_t)sb;

	FileIO_errorno = sys_fstat(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_fstatat(int fd, const char *path, stat_s *buf, int flag)
{
	struct sys_read_args {
		uint64_t fd;
		uint64_t path;
		uint64_t buf;
        uint64_t flag;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_read = (int(*)(thread * td, sys_read_args * uap))sysents[493].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_read_args uap;
	uap.fd = fd;
    uap.path = (uint64_t)path;
	uap.buf = (uint64_t)buf;
	uap.flag = flag;

	FileIO_errorno = sys_read(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

int sys_getdents(int fd, char *buf, int count)
{
	struct sys_getdents_args {
		uint64_t fd;
		uint64_t buf;
		uint64_t count;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_getdents = (int(*)(thread * td, sys_getdents_args * uap))sysents[272].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_getdents_args uap;
	uap.fd = fd;
	uap.buf = (uint64_t)buf;
	uap.count = count;

	FileIO_errorno = sys_getdents(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}

off_t sys_lseek(int fildes, off_t offset, int whence)
{
	struct sys_lseek_args {
		uint64_t fildes;
		uint64_t offset;
		uint64_t whence;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_lseek = (int(*)(thread * td, sys_lseek_args * uap))sysents[478].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_lseek_args uap;
	uap.fildes = fildes;
	uap.offset = offset;
	uap.whence = whence;

	FileIO_errorno = sys_lseek(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (off_t)td->td_retval[0];
}

int getSandboxDirectory(char *destination, int *length)
{
	struct getSandboxDirectory_args {
		uint64_t unk;
		uint64_t destination;
		uint64_t length;
	};

    sysentvec* sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto getSandboxDirectory = (int(*)(thread * td, getSandboxDirectory_args * uap))sysents[602].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	getSandboxDirectory_args uap;
	uap.unk = 0;
	uap.destination = (uint64_t)destination;
	uap.length = (uint64_t)length;

	FileIO_errorno = getSandboxDirectory(td, &uap);
	if(FileIO_errorno)
		return -FileIO_errorno;

	return (int)td->td_retval[0];
}