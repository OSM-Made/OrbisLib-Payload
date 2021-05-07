#include "../../Main.hpp"
#include "KlogHelper.hpp"
#include "../FileIO.hpp"

// Bsed on the work of kiwidog
//
// Credit: https://github.com/OpenOrbis/mira-project/blob/master/kernel/src/Plugins/TTYRedirector/TTYRedirector.hpp
// Kiwidog
// 

KLogHelper::KLogHelper()
{
    uint64_t CR0 = __readcr0();
 	__writecr0(CR0 & ~CR0_WP);

    m_DeciTTYWrite_orig = *(void**)(resolve(0x019FC168));
    *(void**)(resolve(0x019FC168)) = reinterpret_cast<void*>(OnDeciTTYWrite);

    __writecr0(CR0);

    kproc_kthread_add(LoggerThread, this, &orbisLib->kOrbisProc, NULL, NULL, 0, "OrbisLib.elf", "Logger Thread");
}

KLogHelper::~KLogHelper()
{
    if (m_DeciTTYWrite_orig) 
    {
        uint64_t CR0 = __readcr0();
 	    __writecr0(CR0 & ~CR0_WP);

        *(void**)(resolve(0x019FC168)) = m_DeciTTYWrite_orig;

        __writecr0(CR0);
    }
}

void KLogHelper::LoggerClientThread(void* arg)
{
    ClientThreadArgs* clientThreadArgs = (ClientThreadArgs*)arg;
    int Socket = ((ClientThreadArgs*)arg)->Socket;
    auto bytesRead = 0;
    char s_Buffer[2] = { 0 };

    auto s_LogDevice = sys_fopen("/dev/klog", 0x00, 0);
    if (s_LogDevice < 0)
    {
        DebugLog(LOGTYPE_ERR, "Failed to open log device...\n");
        goto End;
    }

    while ((bytesRead = sys_fread(s_LogDevice, s_Buffer, 1)) > 0)
    {
        if (Send(Socket, s_Buffer, 1) <= 0)
            break;

        memset(s_Buffer, 0, sizeof(s_Buffer));
    }

End:
    sys_fclose(s_LogDevice);
    sys_close(Socket);
    kthread_exit();
}

void KLogHelper::LoggerThread(void* arg)
{
    //Set up socket
    int ClientSocket = -1;
	int ServerSocket = sys_socket(AF_INET, SOCK_STREAM, 0);

    int optval = 1;
	sys_setsockopt(ServerSocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&optval, sizeof(int));

    //Bind and start listening on port 9998 with a back log of 100.
    sockaddr_in servaddr = { 0 };
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(9998);

    sys_bind(ServerSocket, (struct sockaddr *)&servaddr, sizeof(servaddr));
	sys_listen(ServerSocket, 100);

    DebugLog(LOGTYPE_INFO, "Logger Started listening on port 9998...");

    //Loop
    while(orbisLib->IsRunning)
    {
        kthread_suspend_check();
        
        //Wait for incoming connections.
        sockaddr_in clientaddr = { 0 };
		int addrlen = sizeof(struct sockaddr_in);
		ClientSocket = sys_accept(ServerSocket, (struct sockaddr *)&clientaddr, &addrlen);

        if (ClientSocket != -1) 
		{
			DebugLog(LOGTYPE_INFO, "New Logger Connection (%i.%i.%i.%i)", clientaddr.sin_addr.s_addr & 0xFF, (clientaddr.sin_addr.s_addr >> 8) & 0xFF, (clientaddr.sin_addr.s_addr >> 16) & 0xFF, (clientaddr.sin_addr.s_addr >> 24) & 0xFF);

			int optval = 1;
			sys_setsockopt(ClientSocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&optval, sizeof(int));

            
            //Add a thread to handle the new client API Request.
            ClientThreadArgs ClientArgs = { ClientSocket }; //clientaddr.sin_addr.s_addr
			kproc_kthread_add(LoggerClientThread, &ClientArgs, &orbisLib->kOrbisProc, NULL, NULL, 0, "OrbisLib.elf", "Logger Client Thread");

            //Reset the temp socket for the next connection.
			ClientSocket = -1;
		}
    }

    DebugLog(LOGTYPE_NONE, "kproc Exiting!!!");
	sys_close(ServerSocket);
    kproc_exit(0);
}

int KLogHelper::OnDeciTTYWrite(struct cdev* dev, struct uio* uio, int ioflag)
{
    struct uio* cloned_uio = NULL;
   int ret;

   auto cloneuio = (struct uio*(*)(struct uio* uiop))resolve(0x002A82E0);
   auto console_write = (int(*)(struct cdev* dev, struct uio* uio, int ioflag))resolve(0x000ECAC0);
   auto deci_tty_write = (int(*)(struct cdev* dev, struct uio* uio, int ioflag))resolve(0x0049CEF0);
   auto M_IOV = (struct malloc_type*)resolve(0x014B5E80);
   auto console_cdev = (struct cdev**)resolve(0x01AC5158);

   cloned_uio = cloneuio(uio);

   ret = deci_tty_write(dev, uio, ioflag);

   if (cloned_uio) {
       if (*console_cdev)
           console_write(*console_cdev, cloned_uio, ioflag);
       free(cloned_uio, M_IOV);
   }

   return ret;
}