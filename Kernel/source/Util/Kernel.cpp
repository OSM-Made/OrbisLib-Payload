#include "../main.hpp"
#include "Kernel.hpp"
#include "FileIO.hpp"

extern "C"
{
	#include <sys/proc.h>
    #include <sys/sysent.h>
    #include <sys/syscall.h>
	#include "sys/sysproto.h"
}

int kern_errorno = 0;

caddr_t kmmap(caddr_t addr, size_t len, int prot, int flags, int fd, off_t pos) 
{
	struct sys_mmap_args {
		uint64_t addr;
		uint64_t len;
		uint64_t prot;
		uint64_t flags;
		uint64_t fd;
		uint64_t pos;
	};

    auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_mmap = (int(*)(thread * td, sys_mmap_args * uap))sysents[SYS_mmap].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_mmap_args uap;
	uap.addr = (uint64_t)addr;
	uap.len = len;
	uap.prot = prot;
	uap.flags = flags;
	uap.fd = fd;
	uap.pos = pos;

	kern_errorno = sys_mmap(td, &uap);

	return (caddr_t)td->td_retval[0];
}

int kmunmap(caddr_t addr, size_t len) 
{
	struct sys_munmap_args {
		uint64_t addr;
		uint64_t len;
	};

    auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_munmap = (int(*)(thread * td, sys_munmap_args* uap))sysents[SYS_munmap].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_munmap_args uap;
	uap.addr = (uint64_t)addr;
	uap.len = len;

	kern_errorno = sys_munmap(td, &uap);

	return td->td_retval[0];
}

int kern_reboot(int opt) 
{
	struct reboot_args {
		uint64_t opt;
	};

    auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_reboot = (int(*)(thread * td, reboot_args* uap))sysents[37].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	reboot_args uap;
	uap.opt = (uint64_t)opt;

	kern_errorno = sys_reboot(td, &uap);

	return td->td_retval[0];
}

int kwait4(int pid, int *status, int options, struct rusage *rusage)
{
	struct kwait4_args {
		uint64_t pid;
		uint64_t status;
		uint64_t options;
		uint64_t rusage;
	};

    auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_kwait4 = (int(*)(thread * td, kwait4_args* uap))sysents[SYS_wait4].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	kwait4_args uap;
	uap.pid = pid;
	uap.status = (uint64_t)status;
	uap.options = options;
	uap.rusage = (uint64_t)rusage;

	kern_errorno = sys_kwait4(td, &uap);
	if(kern_errorno)
		return -kern_errorno;

	return td->td_retval[0];
}

int ksetuid(uid_t uid, thread* td)
{
	auto sv = (sysentvec*)resolve(addr_sysvec);
	sysent* sysents = sv->sv_table;
	auto sys_setuid = (int(*)(thread *, setuid_args *))sysents[23].sy_call;

	setuid_args uap;

	// clear errors
	td->td_retval[0] = 0;

	// call syscall
	uap.uid = uid;

	kern_errorno = sys_setuid(td, &uap);
	if(kern_errorno)
		return -kern_errorno;

	// success
	return td->td_retval[0];
}

int kdup2(int oldd, int newd, struct thread* td)
{
	auto sv = (sysentvec*)resolve(addr_sysvec);
 	sysent* sysents = sv->sv_table;
	auto sys_dup2 = (int(*)(thread *, dup2_args *))sysents[90].sy_call;

	struct dup2_args uap;

	// clear errors
	td->td_retval[0] = 0;

	// call syscall
	uap.from = oldd;
	uap.to = newd;

	kern_errorno = sys_dup2(td, &uap);
	if(kern_errorno)
		return -kern_errorno;

	// success
	return td->td_retval[0];
}

uint64_t GetCPUTemp()
{
	uint64_t temp = 0;
	int fd = sys_fopen("/dev/sbi", 0, 0);

	if(fd)
	{
		kernel_ioctl(curthread(), fd, 0x4004A501, &temp);

		sys_fclose(fd);
	}

	return temp;
}

uint64_t GetSOCTemp()
{
	uint64_t temp = 0;
	int fd = sys_fopen("/dev/sbi", 2, 0);

	if(fd)
	{
		kernel_ioctl(curthread(), fd, 0xC008A502, &temp);

		sys_fclose(fd);
	}

	return temp >> 32;
}

void IccIndicatorBootDone()
{
	unsigned int device = sys_fopen("/dev/icc_indicator", 0x10002, 0);

	if(device)
	{
		kernel_ioctl(curthread(), device, 0x20009506);

		sys_fclose(device);
	}
}

void IccIndicatorShutdown()
{
	unsigned int device = sys_fopen("/dev/icc_indicator", 0x10002, 0);

	if(device)
	{
		kernel_ioctl(curthread(), device, 0x20009507);

		sys_fclose(device);
	}
}

void IccIndicatorStandby()
{
	unsigned int device = sys_fopen("/dev/icc_indicator", 0x10002, 0);

	if(device)
	{
		kernel_ioctl(curthread(), device, 0x20009508);

		sys_fclose(device);
	}
}

void sceKernelIccIndicatorStandbyShutdown()
{
	unsigned int device = sys_fopen("/dev/icc_indicator", 0x10002, 0);

	if(device)
	{
		kernel_ioctl(curthread(), device, 0x20009509);

		sys_fclose(device);
	}
}

void sceKernelIccIndicatorStandbyBoot()
{
	unsigned int device = sys_fopen("/dev/icc_indicator", 0x10002, 0);

	if(device)
	{
		kernel_ioctl(curthread(), device, 0x2000950A);

		sys_fclose(device);
	}
}

void sceKernelIccIndicatorUNK()
{
	char unk = 0;
	unsigned int device = sys_fopen("/dev/icc_indicator", 0x10002, 0);

	if(device)
	{
		kernel_ioctl(curthread(), device, 0x80019501, &unk);

		sys_fclose(device);
	}
}

int32_t sys_evf_open(const char* evf)
{
	struct sys_evf_open_args {
		uint64_t evf;
	};

	auto sv = (sysentvec*)resolve(addr_sysvec);
 	sysent* sysents = sv->sv_table;
	auto sys_evf_open = (int(*)(thread *, sys_evf_open_args *))sysents[540].sy_call;

	struct sys_evf_open_args uap;

	thread *td = curthread();

	// clear errors
	td->td_retval[0] = 0;

	// call syscall
	uap.evf = (uint64_t)evf;

	kern_errorno = sys_evf_open(td, &uap);
	if(kern_errorno)
		return -kern_errorno;

	// success
	return td->td_retval[0];
}

int32_t sys_evf_cancel(int32_t evf, int64_t unk, int64_t unk2)
{
	struct sys_evf_cancel_args {
		uint64_t evf;
		uint64_t unk;
		uint64_t unk2;
	};

	auto sv = (sysentvec*)resolve(addr_sysvec);
 	sysent* sysents = sv->sv_table;
	auto sys_evf_cancel = (int(*)(thread *, sys_evf_cancel_args *))sysents[546].sy_call;

	thread *td = curthread();

	struct sys_evf_cancel_args uap;

	// clear errors
	td->td_retval[0] = 0;

	// call syscall
	uap.evf = evf;
	uap.unk = unk;
	uap.unk2 = unk2;

	kern_errorno = sys_evf_cancel(td, &uap);
	if(kern_errorno)
		return -kern_errorno;

	// success
	return td->td_retval[0];
}

int32_t sys_evf_close(int32_t evf)
{
	struct sys_evf_close_args {
		uint64_t evf;
	};

	auto sv = (sysentvec*)resolve(addr_sysvec);
 	sysent* sysents = sv->sv_table;
	auto sys_evf_close = (int(*)(thread *, sys_evf_close_args *))sysents[540].sy_call;

	thread *td = curthread();

	struct sys_evf_close_args uap;

	// clear errors
	td->td_retval[0] = 0;

	// call syscall
	uap.evf = evf;

	kern_errorno = sys_evf_close(td, &uap);
	if(kern_errorno)
		return -kern_errorno;

	// success
	return td->td_retval[0];
}

int32_t sys_kill(int32_t pid, int sig)
{
	auto sv = (sysentvec*)resolve(addr_sysvec);
 	sysent* sysents = sv->sv_table;
	auto sys_kill = (int(*)(thread *, kill_args *))sysents[37].sy_call;

	thread *td = curthread();

	struct kill_args uap;

	// clear errors
	td->td_retval[0] = 0;

	// call syscall
	uap.pid = pid;
	uap.signum = sig;

	kern_errorno = sys_kill(td, &uap);
	if(kern_errorno)
		return -kern_errorno;

	// success
	return td->td_retval[0];
}

void kReboot()
{
	int evf = sys_evf_open("SceSysCoreReboot");
	sys_evf_cancel(evf, 0x0, 0);
	sys_evf_close(evf);
	sys_kill(1, 30);
}

void kShutdown()
{
	int evf = sys_evf_open("SceSysCoreReboot");
	sys_evf_cancel(evf, 0x4000, 0);
	sys_evf_close(evf);
	sys_kill(1, 30);
}

void kSuspend()
{
	int evf = sys_evf_open("SceSysCoreReboot");
	sys_evf_cancel(evf, 0x8004000, 0);
	sys_evf_close(evf);
	sys_kill(1, 30);
}

enum NotifyType
{
	NotificationRequest = 0,
	SystemNotification = 1,
	SystemNotificationWithUserId = 2,
	SystemNotificationWithDeviceId = 3,
	SystemNotificationWithDeviceIdRelatedToUser = 4,
	SystemNotificationWithText = 5,
	SystemNotificationWithTextRelatedToUser = 6,
	SystemNotificationWithErrorCode = 7,
	SystemNotificationWithAppId = 8,
	SystemNotificationWithAppName = 9,
	SystemNotificationWithAppInfo = 9,
	SystemNotificationWithAppNameRelatedToUser = 10,
	SystemNotificationWithParams = 11,
	SendSystemNotificationWithUserName = 12,
	SystemNotificationWithUserNameInfo = 13,
	SendAddressingSystemNotification = 14,
	AddressingSystemNotificationWithDeviceId = 15,
	AddressingSystemNotificationWithUserName = 16,
	AddressingSystemNotificationWithUserId = 17,

	UNK_1 = 100,
	TrcCheckNotificationRequest = 101,
	NpDebugNotificationRequest = 102,
	UNK_2 = 102,
};

struct NotifyBuffer
{
	NotifyType Type;		//0x00 
	int ReqId;				//0x04
	int Priority;			//0x08
	int MsgId;				//0x0C
	int TargetId;			//0x10
	int UserId;				//0x14
	int unk1;				//0x18
	int unk2;				//0x1C
	int AppId;				//0x20
	int ErrorNum;			//0x24
	int unk3;				//0x28
	char UseIconImageUri; 	//0x2C
	char Message[1024]; 	//0x2D
	char Uri[1024]; 		//0x42D
	char unkstr[1024];		//0x82D
}; //Size = 0xC30 

void SceNotify(char* IconUri, const char* fmt, ...)
{
	NotifyBuffer Buffer;
	unsigned int device = -1;

	//Create full string from va list
	va_list args;
	va_start(args, fmt);
	vsprintf(Buffer.Message, fmt, args);
	va_end(args);

	Buffer.Type = NotifyType::NotificationRequest;
	Buffer.unk3 = 0;
	Buffer.UseIconImageUri = 1;
	Buffer.TargetId = -1;
	strcpy(Buffer.Uri, IconUri);

	device = sys_fopen("/dev/notification0", 1, 0);
	if(!device)
		device = sys_fopen("/dev/notification0", 5, 0);

	if(!device)
		device = sys_fopen("/dev/notification1", 1, 0);

	if(!device)
		device = sys_fopen("/dev/notification1", 5, 0);

	if(device)
	{
		sys_fwrite(device, &Buffer, 3120);

		sys_fclose(device);
	}
}

void SceNotify(const char* fmt, ...)
{
	NotifyBuffer Buffer;
	unsigned int device = -1;

	//Create full string from va list
	va_list args;
	va_start(args, fmt);
	vsprintf(Buffer.Message, fmt, args);
	va_end(args);

	Buffer.Type = NotifyType::NotificationRequest;
	Buffer.unk3 = 0;
	Buffer.UseIconImageUri = 1;
	Buffer.TargetId = -1;
	strcpy(Buffer.Uri, "https://i.imgur.com/SJPIBGG.png");

	device = sys_fopen("/dev/notification0", 1, 0);
	if(!device)
		device = sys_fopen("/dev/notification0", 5, 0);

	if(!device)
		device = sys_fopen("/dev/notification1", 1, 0);

	if(!device)
		device = sys_fopen("/dev/notification1", 5, 0);

	if(device)
	{
		sys_fwrite(device, &Buffer, 3120);

		sys_fclose(device);
	}
}

/*struct SceFsMountGameDataOpt
{
	const char* pfsImgFile; 		//0x00

};

struct pConfig
{
	uint64_t VolumeSize;	//0x18
};

int sceFsMountGamePkg(
	SceFsMountGameDataOpt* Opt, 
	char* MountPath, 
	int32_t unk1, // Gets stored on the stack is passed into pfsMountData
	pConfig* i_pConfig, //structure of some sort it seems
	int32_t unk3, 
	int32_t unk4
)
{

}*/