#include "../Main.hpp"
#include "OrbisDef.hpp"
#include "OrbisTarget.hpp"

OrbisTarget::OrbisTarget()
{
	DebugLog(LOGTYPE_INFO, "Initialization!!");

}

OrbisTarget::~OrbisTarget()
{
	DebugLog(LOGTYPE_INFO, "Destruction!!");
}

void OrbisTarget::Info(int Socket)
{
    RESP_TargetInfo TargetInfo;

	char PSID[16] = { 0 };
	size_t PSIDlen = 16;
	int ret = kernel_sysctlbyname(curthread(), "machdep.openpsid", (char*)&PSID, &PSIDlen, NULL, NULL, NULL, 0);

	char IDPS[16] = { 0 };
	size_t IDPSlen = 16;
	ret = kernel_sysctlbyname(curthread(), "machdep.idps", (char*)&IDPS, &IDPSlen, NULL, NULL, NULL, 0);

	int ConsoleType = GetConsoleType(IDPS[5] & 0xffU);

	int sdk_version;
	size_t sdk_versionlen = 4;
	ret = kernel_sysctlbyname(curthread(), "kern.sdk_version", (char*)&sdk_version, &sdk_versionlen, NULL, NULL, NULL, 0);

	int upd_version;
	size_t upd_versionlen = 4;
	ret = kernel_sysctlbyname(curthread(), "machdep.upd_version", (char*)&upd_version, &upd_versionlen, NULL, NULL, NULL, 0);

	int32_t CPUTemp = GetCPUTemp();
	int32_t SOCTemp = GetSOCTemp();

    proc* proc = GetCurrentGame();
	if(proc)
		strcpy(TargetInfo.CurrentTitleID, proc->titleId);
	else
	    strcpy(TargetInfo.CurrentTitleID, "XMB");
	
    //Fill Response packet.
    TargetInfo.SDKVersion = sdk_version;
    TargetInfo.SoftwareVersion = upd_version;
	icc_nvs_read(4, 0x60, sizeof(TargetInfo.FactorySoftwareVersion), (unsigned char*)&TargetInfo.FactorySoftwareVersion);
	TargetInfo.CPUTemp = CPUTemp;
	TargetInfo.SOCTemp = SOCTemp;
	sceRegMgrGetStr(0x02050000, (char*)&TargetInfo.ConsoleName, sizeof(TargetInfo.ConsoleName)); // /SYSYEM/nickname
	icc_nvs_read(2, 0, 14, (unsigned char*)&TargetInfo.MotherboardSerial);
	icc_nvs_read(2, 0x30, 10, (unsigned char*)&TargetInfo.Serial);
	icc_nvs_read(2, 0x41, 9, (unsigned char*)&TargetInfo.Model);
	icc_nvs_read(0, 0x21, 6, (unsigned char*)&TargetInfo.MACAdressLAN);
	icc_nvs_read(0, 0x21, 6, (unsigned char*)&TargetInfo.MACAdressWIFI); //TODO: Find
	icc_nvs_read(4, 0x31F, 0x1, (unsigned char*)&TargetInfo.UART);
	icc_nvs_read(4, 0x1600, 0x1, (unsigned char*)&TargetInfo.IDUMode);
    memcpy(TargetInfo.IDPS, IDPS, 16);
    memcpy(TargetInfo.PSID, PSID, 16);
    TargetInfo.ConsoleType = ConsoleType;
	TargetInfo.Attached = (int32_t)orbisLib->orbisProc->CurrentlyAttached;
	strcpy(TargetInfo.CurrentProc, orbisLib->orbisProc->CurrentProcName);

	SendStatus(Socket, API_OK);

	Send(Socket, (char*)&TargetInfo, sizeof(RESP_TargetInfo));
}

void OrbisTarget::Shutdown(int Socket)
{
   DebugLog(LOGTYPE_WARN, "Not Implimented!");
   SendStatus(Socket, API_ERROR_FAIL);
}

void OrbisTarget::Reboot(int Socket)
{
    DebugLog(LOGTYPE_WARN, "Not Implimented!");
	SendStatus(Socket, API_ERROR_FAIL);
}

void OrbisTarget::Notify(int Socket, int Type, char* Message)
{
    if(Type == -1)
        pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest(Message);
    else
        pHelperManager->pUserlandHelper->sceSysUtilSendSystemNotificationWithText(Type, Message);
    
    SendStatus(Socket, API_OK);
}

void OrbisTarget::Beep(int Socket, int Count)
{
    if(Count > 5)
    {
        DebugLog(LOGTYPE_WARN, "Beep count cant be over 5.");

        SendStatus(Socket, API_ERROR_FAIL);

        return;
    }

    auto icc_indicator_set_buzzer = (void(*)(char))resolve(addr_icc_indicator_set_buzzer);
	icc_indicator_set_buzzer(Count);

    SendStatus(Socket, API_OK);
}

void OrbisTarget::SetLED(int Socket)
{
    DebugLog(LOGTYPE_WARN, "Not Implimented!");
	SendStatus(Socket, API_ERROR_FAIL);

    struct
	{
		uint16_t unk; //0x00
		uint16_t unk1; //0x02
		uint16_t unk2; //0x04
		uint16_t unk3; //0x08
		uint16_t unk4; //0x0A
		uint16_t unk5; //0x0C
		uint16_t unk6; //0x0E
		uint16_t unk7; //0x10
		uint16_t unk8; //0x12
		uint16_t unk9; //0x14
		uint16_t unk10; //0x16
		uint16_t unk11; //0x18
	}somethingcolours;

	auto getledsboss = (void(*)(uint64_t*))resolve(0x104B20);
	getledsboss((uint64_t*)&somethingcolours);

	DebugLog(LOGTYPE_INFO, "unk = %llX", somethingcolours.unk);
	DebugLog(LOGTYPE_INFO, "unk1 = %llX", somethingcolours.unk1);
	DebugLog(LOGTYPE_INFO, "unk2 = %llX", somethingcolours.unk2);
	DebugLog(LOGTYPE_INFO, "unk3 = %llX", somethingcolours.unk3);
	DebugLog(LOGTYPE_INFO, "unk4 = %llX", somethingcolours.unk4);
	DebugLog(LOGTYPE_INFO, "unk5 = %llX", somethingcolours.unk5);
	DebugLog(LOGTYPE_INFO, "unk6 = %llX", somethingcolours.unk6);
	DebugLog(LOGTYPE_INFO, "unk7 = %llX", somethingcolours.unk7);
	DebugLog(LOGTYPE_INFO, "unk8 = %llX", somethingcolours.unk8);
	DebugLog(LOGTYPE_INFO, "unk9 = %llX", somethingcolours.unk9);
	DebugLog(LOGTYPE_INFO, "unk10 = %llX", somethingcolours.unk10);
	DebugLog(LOGTYPE_INFO, "unk11 = %llX", somethingcolours.unk11);

	//auto setledsboss = (void(*)(uint64_t*))resolve(0x1049B0);
	//etledsboss((uint64_t*)&somethingcolours);
}

void OrbisTarget::GetLED(int Socket)
{
    DebugLog(LOGTYPE_WARN, "Not Implimented!");

	SendStatus(Socket, API_ERROR_FAIL);
}

void OrbisTarget::DumpProcess(int Socket, const char* ProcessName)
{
	int64_t Size = 0;
	char* DumpedData = 0;
	size_t n = 0;
	int err = 0;
	proc* proc = proc_find_by_name(ProcessName);
	if(!proc)
	{
		DebugLog(LOGTYPE_ERR, "Failed to get process \"%s\".", ProcessName);

		SendStatus(Socket, API_ERROR_FAIL);

		return;
	}

	Size = (proc->p_vmspace->vm_tsize + proc->p_vmspace->vm_dsize) * PAGE_SIZE;

	DumpedData = (char*)_malloc(Size);
	if(!DumpedData)
	{
		DebugLog(LOGTYPE_ERR, "Failed to allocate space for Dump.");
		
		SendStatus(Socket, API_ERROR_FAIL);

		goto Cleanup;
	}

	err = proc_rw_mem(proc, proc->p_vmspace->vm_taddr, proc->p_vmspace->vm_tsize * PAGE_SIZE, DumpedData, &n, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to read Text Segment Data: %d(%d).", err, n);

		SendStatus(Socket, API_ERROR_FAIL);

		goto Cleanup;

	}

	n = 0;
	err = proc_rw_mem(proc, proc->p_vmspace->vm_daddr, proc->p_vmspace->vm_dsize * PAGE_SIZE, DumpedData + (proc->p_vmspace->vm_tsize * PAGE_SIZE), &n, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to read Data Segment Data: %d(%d).", err, n);

		SendStatus(Socket, API_ERROR_FAIL);

		goto Cleanup;
	}

	SendStatus(Socket, API_OK);

	Send(Socket, (char*)&Size, 0x8);
	Send(Socket, DumpedData, Size);

Cleanup:
	_free(DumpedData);

	char Buffer[0x200];
	sprintf(Buffer, "Dumped: %s(0x%llX).", ProcessName, Size);
	pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest(Buffer);
}

void OrbisTarget::APIHandle(int Socket, API_Packet_s* Packet)
{
	switch(Packet->cmd)
	{
	case API_TARGET_INFO:
		Info(Socket);
		break;

	case API_TARGET_SHUTDOWN:
		Shutdown(Socket);
		break;

	case API_TARGET_REBOOT:
		Reboot(Socket);
		break;

	case API_TARGET_NOTIFY:
		Notify(Socket, Packet->Target_Notify.MessageType, Packet->Target_Notify.Message);
		break;

	case API_TARGET_BEEP:
		Beep(Socket, Packet->Target_Beep.Count);
		break;

	case API_TARGET_SET_LED:
		SetLED(Socket);
		break;

	case API_TARGET_GET_LED:
		GetLED(Socket);
		break;

	case API_TARGET_DUMP_PROC:
		DumpProcess(Socket, Packet->ProcName);
		break;
	}

}