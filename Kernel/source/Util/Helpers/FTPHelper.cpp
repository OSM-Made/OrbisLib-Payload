#include "../../Main.hpp"
#include "FTPHelper.hpp"

extern uint8_t OrbisFTP[];
extern int32_t OrbisFTPSize;

void LoadFTP(const char* ProcessName)
{
    proc* proc = proc_find_by_name(ProcessName);//SceRemotePlay SceShellUI
	if(proc) 
	{
        //Give Root FS Perms
        /*ucred* cred = proc->p_ucred;
        if(cred)
        {
            cred->cr_prison = *(prison**)resolve(addr_prison0);

            cred->cr_uid = 0;
            cred->cr_ruid = 0;
            cred->cr_rgid = 0;
            cred->cr_groups[0] = 0;
        }*/

		filedesc* fd = proc->p_fd;
        if(fd)
        {
            fd->fd_jdir = *(vnode**)resolve(addr_rootvnode);
		    fd->fd_rdir = *(vnode**)resolve(addr_rootvnode);
        }

		proc_LoadELF(proc, (char*)OrbisFTP);
	}
}