#pragma once

class KLogHelper
{
    private:
        void* m_DeciTTYWrite_orig;

    public:
        KLogHelper();
        ~KLogHelper();

        struct ClientThreadArgs
        {
            int Socket;
        };

        static void LoggerClientThread(void* arg);
        static void LoggerThread(void* arg);
        static int OnDeciTTYWrite(struct cdev* dev, struct uio* uio, int ioflag);
};