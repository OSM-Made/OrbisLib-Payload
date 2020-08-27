#pragma once

enum {
	SEEK_SET,
	SEEK_CUR,
	SEEK_END
};

#define O_RDONLY  0x0000
#define O_WRONLY  0x0001
#define O_RDWR    0x0002
#define O_ACCMODE 0x0003

#define	O_NONBLOCK 0x0004		/* no delay */
#define	O_APPEND   0x0008		/* set append mode */
#define	O_CREAT    0x0200		/* create if nonexistent */
#define	O_TRUNC    0x0400		/* truncate to zero length */
#define	O_EXCL     0x0800		/* error if already exists */

#define S_ISDIR(m)  (((m) & 0170000) == 0040000)
#define S_ISCHR(m)  (((m) & 0170000) == 0020000)
#define S_ISBLK(m)  (((m) & 0170000) == 0060000)
#define S_ISREG(m)  (((m) & 0170000) == 0100000)
#define S_ISFIFO(m) (((m) & 0170000) == 0010000)
#define S_ISLNK(m)  (((m) & 0170000) == 0120000)
#define S_ISSOCK(m) (((m) & 0170000) == 0140000)
#define S_ISWHT(m)  (((m) & 0170000) == 0160000)

struct stat_s {
	uint32_t   st_dev;		/* inode's device */
	ino_t	  st_ino;		/* inode's number */
	mode_t	  st_mode;		/* inode protection mode */
	nlink_t	  st_nlink;		/* number of hard links */
	uid_t	  st_uid;		/* user ID of the file's owner */
	gid_t	  st_gid;		/* group ID of the file's group */
	uint32_t   st_rdev;		/* device type */
	struct	timespec st_atim;	/* time of last access */
	struct	timespec st_mtim;	/* time of last data modification */
	struct	timespec st_ctim;	/* time of last file status change */
	off_t	  st_size;		/* file size, in bytes */
	blkcnt_t st_blocks;		/* blocks allocated for file */
	blksize_t st_blksize;		/* optimal blocksize for I/O */
	fflags_t  st_flags;		/* user defined flags for file */
	uint32_t st_gen;		/* file generation number */
	int32_t st_lspare;
	struct timespec st_birthtim;	/* time of file creation */
	unsigned int :(8 / 2) * (16 - (int)sizeof(struct timespec));
	unsigned int :(8 / 2) * (16 - (int)sizeof(struct timespec));
};

ssize_t sys_fread(int fd, void *buf, size_t nbyte);
ssize_t sys_fwrite(int fd, const void *buf, size_t count);
int sys_fopen(char *path, int flags, int mode);
int sys_fclose(int fd);
int sys_unlink(const char *pathname);
int sys_readlink(const char *path,	char *buf, int buflen);
int sys_mount(const char *type, const char	*dir, int flags, void *data);
int sys_nmount(struct iovec *iov, uint32_t niov, int flags);
int sys_unmount(const char *dir, int flags);
int sys_fchown(int fd, int uid, int gid);
int sys_fchmod(int fd, int mode);
int sys_rename(const char *oldpath, const char *newpath);
int sys_mkdir(char *pathname, int mode);
int sys_rmdir(const char *path);
int sys_stat(char *path, struct stat *sb);
int sys_fstat(int fd, struct stat *sb);
int sys_fstatat(int fd, const char *path, stat_s *buf, int flag);
int sys_getdents(int fd, char *buf, int count);
off_t sys_lseek(int fildes, off_t offset, int whence);
int getSandboxDirectory(char *destination, int *length);
