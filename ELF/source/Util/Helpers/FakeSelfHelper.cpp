#include "../../Main.hpp"
#include "FakeSelfHelper.hpp"

/*
Credits:

    Implemented from: https://github.com/xvortex/ps4-hen-vtx
    Ported by: kiwidog (@kd_tech_)
    Bugfixes: SiSTRo (https://github.com/SiSTR0), SocraticBliss (https://github.com/SocraticBliss)

*/

const uint8_t FakeSelfHelper::s_auth_info_for_exec[] =
{
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x20,
  0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,
  0x00, 0x40, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t FakeSelfHelper::s_auth_info_for_dynlib[] =
{
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x30, 0x00, 0x30,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,
  0x00, 0x40, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

SblMapListEntry* FakeSelfHelper::SceSblDriverFindMappedPageListByGpuVa(vm_offset_t p_GpuVa)
{
    if (p_GpuVa == 0)
    {
        DebugLog(LOGTYPE_ERR, "invalid gpu va");
        return nullptr;
    }

    auto s_SblDrvMsgMtx = (struct mtx*)resolve(addr_sbl_drv_msg_mtx);

    SblMapListEntry* s_Entry = *(SblMapListEntry**)resolve(addr_gpu_va_page_list);
    SblMapListEntry* s_FinalEntry = nullptr;

    // Lock before we iterate this list, because other paths can absolutely use it concurrently
    _mtx_lock_flags(s_SblDrvMsgMtx, 0, __FILE__, __LINE__);

    while (s_Entry)
    {
        if (s_Entry->gpuVa == p_GpuVa)
        {
            s_FinalEntry = s_Entry;
            break;
        }

        s_Entry = s_Entry->next;
    }

    _mtx_unlock_flags(s_SblDrvMsgMtx, 0, __FILE__, __LINE__);
    return s_FinalEntry;
}

vm_offset_t FakeSelfHelper::SceSblDriverGpuVaToCpuVa(vm_offset_t p_GpuVa, size_t* p_NumPageGroups)
{
    SblMapListEntry* s_Entry = SceSblDriverFindMappedPageListByGpuVa(p_GpuVa);
    if (s_Entry == nullptr)
    {
        DebugLog(LOGTYPE_ERR, "invalid gpu va entry");
        return 0;
    }
    
    if (p_NumPageGroups != nullptr)
        *p_NumPageGroups = s_Entry->numPageGroups;
    
    return s_Entry->cpuVa;
}

bool FakeSelfHelper::IsFakeSelf(SelfContext* pSelfContext)
{
    if (pSelfContext == nullptr)
    {
        DebugLog(LOGTYPE_ERR, "pSelfContext was NULL");
        return false;
    }
    
    SelfExInfo* s_Info = nullptr;
    if (pSelfContext != nullptr && pSelfContext->format == SelfFormat::Self)
    {
        (void)sceSblAuthMgrGetSelfInfo(pSelfContext, &s_Info);
        
        //WriteLog(LL_Debug, "ptype: (%d)", s_Info->ptype);
        return (int32_t)s_Info->ptype == 1;
    }
    else
        return false;
}

int FakeSelfHelper::SceSblAuthMgrGetElfHeader(SelfContext* pSelfContext, Elf64_Ehdr** pOutElfHeader)
{
    if (pSelfContext == nullptr)
        return -EAGAIN;
    
    if (pSelfContext->format == SelfFormat::Elf)
    {
        DebugLog(LOGTYPE_INFO, "elf format");
        auto s_ElfHeader = reinterpret_cast<Elf64_Ehdr*>(pSelfContext->header);
        if (s_ElfHeader != nullptr)
            *pOutElfHeader = s_ElfHeader;
        
        return 0;
    }
    else if (pSelfContext->format == SelfFormat::Self)
    {
        DebugLog(LOGTYPE_INFO, "self format");
        auto s_SelfHeader = reinterpret_cast<SelfHeader*>(pSelfContext->header);
        size_t s_PdataSize = s_SelfHeader->headerSize - sizeof(SelfEntry) * s_SelfHeader->numEntries - sizeof(SelfHeader);
        if (s_PdataSize >= sizeof(Elf64_Ehdr) && (s_PdataSize & 0xF) == 0)
        {
            auto s_ElfHeader = reinterpret_cast<Elf64_Ehdr*>((uint8_t*)s_SelfHeader + sizeof(SelfHeader) + sizeof(SelfEntry) * s_SelfHeader->numEntries);
            if (s_ElfHeader)
                *pOutElfHeader = s_ElfHeader;
            
            return 0;
        }

        DebugLog(LOGTYPE_ERR, "-EALREADY");
        return -EALREADY;
    }

    DebugLog(LOGTYPE_ERR, "-EAGAIN");
    return -EAGAIN;
}

int FakeSelfHelper::SceSblAuthMgrGetSelfAuthInfoFake(SelfContext* pSelfContext, SelfAuthInfo* pInfo)
{
    if (pSelfContext == nullptr)
    {
        DebugLog(LOGTYPE_ERR, "invalid context");
        return -EAGAIN;
    }
    
    if (pSelfContext->format == SelfFormat::Elf)
    {
        DebugLog(LOGTYPE_ERR, "invalid format");
        return -EAGAIN;
    }
    
    SelfHeader* s_Header = (SelfHeader*)pSelfContext->header;
    auto s_Data = reinterpret_cast<const char*>(pSelfContext->header);
    auto s_FakeInfo = reinterpret_cast<const SelfFakeAuthInfo*>(s_Data + s_Header->headerSize + s_Header->metaSize - 0x100);
    if (s_FakeInfo->size == sizeof(s_FakeInfo->info))
    {
        memcpy(pInfo, &s_FakeInfo->info, sizeof(*pInfo));
        return 0;
    }

    //DebugLog(LOGTYPE_ERR, "EALREADY (no valid authinfo)");
    return -EALREADY;
}

int FakeSelfHelper::BuildFakeSelfAuthInfo(SelfContext* pSelfContext, SelfAuthInfo* pParentAuthInfo, SelfAuthInfo* pAuthInfo)
{
    if (pSelfContext == nullptr || pParentAuthInfo == nullptr || pAuthInfo == nullptr)
    {
        DebugLog(LOGTYPE_ERR, "invalid context (%p) || parentAuthInfo (%p) || authInfo (%p)", pSelfContext, pParentAuthInfo, pAuthInfo);
        return -EINVAL;
    }
    
    if (!IsFakeSelf(pSelfContext))
    {
        DebugLog(LOGTYPE_ERR, "not fake self");
        return -EINVAL;
    }

    SelfExInfo* s_ExInfo = nullptr;
    int32_t s_Result = sceSblAuthMgrGetSelfInfo(pSelfContext, &s_ExInfo);
    if (s_Result)
    {
        DebugLog(LOGTYPE_ERR, "could not get self info (%d).", s_Result);
        return s_Result;
    }

    Elf64_Ehdr* sElfHeader = nullptr;
    s_Result = SceSblAuthMgrGetElfHeader(pSelfContext, &sElfHeader);
    if (s_Result)
    {
        DebugLog(LOGTYPE_ERR, "could not get elf header (%d).", s_Result);
        return s_Result;
    }

    if (sElfHeader == nullptr)
    {
        DebugLog(LOGTYPE_ERR, "elf header invalid");
        return -ESRCH;
    }
    
    SelfAuthInfo s_Info = { 0 };
    s_Result = SceSblAuthMgrGetSelfAuthInfoFake(pSelfContext, &s_Info);
    if (s_Result)
    {
        switch (sElfHeader->e_type)
        {
        case ET_EXEC:
        case ET_SCE_EXEC:
        case ET_SCE_EXEC_ASLR:
            memcpy(&s_Info, s_auth_info_for_exec, sizeof(s_Info));
            s_Result = 0;
            break;
        case ET_SCE_DYNAMIC:
            memcpy(&s_Info, s_auth_info_for_dynlib, sizeof(s_Info));
            s_Result = 0;
            break;
        default:
            s_Result = ENOTSUP;
            return s_Result;
        }

        s_Info.paid = s_ExInfo->paid;
    }

    if (pAuthInfo)
        memcpy(pAuthInfo, &s_Info, sizeof(*pAuthInfo));

    return s_Result;
}

int FakeSelfHelper::AuthSelfHeader(SelfContext* pContext)
{    
    bool s_IsUnsigned = pContext->format == SelfFormat::Elf || IsFakeSelf(pContext);
    if (s_IsUnsigned)
    {
        DebugLog(LOGTYPE_INFO, "fixing unsigned");
        auto s_OldFormat = pContext->format;
        auto s_OldTotalHeaderSize = pContext->totalHeaderSize;

        // Get the mini-syscore.elf binary
        SelfHeader* s_Header = (SelfHeader*)resolve(addr_mini_syscore_self_binary);

        auto s_NewTotalHeaderSize = s_Header->headerSize + s_Header->metaSize;

        // Allocate some memory to hold our header size
        auto s_Temp = new uint8_t[s_NewTotalHeaderSize];
        if (s_Temp == nullptr)
        {
            DebugLog(LOGTYPE_ERR, "could not allocate new total header size (%x).", s_NewTotalHeaderSize);
            return ENOMEM;
        }

        // Backup our current header
        memcpy(s_Temp, pContext->header, s_NewTotalHeaderSize);

        // Copy over mini-syscore.elf's header
        memcpy(pContext->header, s_Header, s_NewTotalHeaderSize);

        // Change the format
        pContext->format = SelfFormat::Self;
        pContext->totalHeaderSize = s_NewTotalHeaderSize;

        // xxx: call the original method using a real SELF file
        auto s_Result = sceSblAuthMgrVerifyHeader(pContext);

        // Restore everything
        memcpy(pContext->header, s_Temp, s_NewTotalHeaderSize);
        pContext->format = s_OldFormat;
        pContext->totalHeaderSize = s_OldTotalHeaderSize;

        delete [] s_Temp;

        return s_Result;
    }
    else
        return sceSblAuthMgrVerifyHeader(pContext);
}


int FakeSelfHelper::SceSblAuthMgrVerifyHeaderHook(SelfContext* pSelfContext)
{
    void* s_Temp = nullptr;
    sceSblAuthMgrSmStart(&s_Temp);

    return AuthSelfHeader(pSelfContext);
}

int FakeSelfHelper::SceSblAuthMgrIsLoadable2Hook(SelfContext* pSelfContext, SelfAuthInfo* pOldAuthInfo, int32_t pPathId, SelfAuthInfo* pNewAuthInfo)
{
    if (pSelfContext == nullptr)
    {
        DebugLog(LOGTYPE_ERR, "pSelfContext was NULL");
        return sceSblAuthMgrIsLoadable2(pSelfContext, pOldAuthInfo, pPathId, pNewAuthInfo);
    } 
    
    if (pSelfContext->format == SelfFormat::Elf || IsFakeSelf(pSelfContext))
    {
        DebugLog(LOGTYPE_INFO, "building fake self information");
        return BuildFakeSelfAuthInfo(pSelfContext, pOldAuthInfo, pNewAuthInfo);
    }        
    else
        return sceSblAuthMgrIsLoadable2(pSelfContext, pOldAuthInfo, pPathId, pNewAuthInfo);
}

int FakeSelfHelper::SceSblAuthMgrSmLoadSelfSegment_MailboxHook(uint64_t pServiceId, void* pRequest, void* pResponse)
{
	// self_context is first param of caller. 0x08 = sizeof(struct self_context*)
	uint8_t* frame = (uint8_t*)__builtin_frame_address(1);
	SelfContext* s_Context = *(SelfContext**)(frame - 0x08);

    auto s_RequestMessage = static_cast<MailboxMessage*>(pRequest);
    if (s_RequestMessage == nullptr)
    {
        DebugLog(LOGTYPE_INFO, "invalid response");
        return sceSblServiceMailbox(pServiceId, pRequest, pResponse);
    }

    if (s_Context == nullptr)
    {
        DebugLog(LOGTYPE_INFO, "could not load segment, could not get self context.");
        return sceSblServiceMailbox(pServiceId, pRequest, pResponse);
    }

    bool s_IsUnsigned = s_Context && IsFakeSelf(s_Context);
    if (s_IsUnsigned)
    {
        DebugLog(LOGTYPE_INFO, "unsigned/fake (s)elf detected clearing ret val");
        s_RequestMessage->retVal = 0;
        return 0;
    }
    
    return sceSblServiceMailbox(pServiceId, pRequest, pResponse);
}

int FakeSelfHelper::SceSblAuthMgrSmLoadSelfBlock_MailboxHook(uint64_t pServiceId, uint8_t* pRequest, void* pResponse)
{
    // self_context is first param of caller. 0x08 = sizeof(struct self_context*)
    uint8_t* frame = (uint8_t*)__builtin_frame_address(1);
    SelfContext* pContext = *(SelfContext**)(frame - 0x08);

    bool s_IsUnsigned = pContext && (pContext->format == SelfFormat::Elf || IsFakeSelf((SelfContext*)pContext));

    if (!s_IsUnsigned) {
        return sceSblServiceMailbox(pServiceId, pRequest, pResponse);
    } else {
        
        vm_offset_t s_SegmentDataGpuVa = *(uint64_t*)(pRequest + 0x08);
        vm_offset_t s_CurrentDataGpuVa = *(uint64_t*)(pRequest + 0x50);
        vm_offset_t s_CurrentData2GpuVa = *(uint64_t*)(pRequest + 0x58);

        uint32_t s_DataOffset = *(uint32_t*)(pRequest + 0x44);
        uint32_t s_DataSize = *(uint32_t*)(pRequest + 0x48);

        /* looking into lists of GPU's mapped memory regions */
        vm_offset_t s_SegmentDataCpuVa = SceSblDriverGpuVaToCpuVa(s_SegmentDataGpuVa, NULL);
        vm_offset_t s_CurrentDataCpuVa = SceSblDriverGpuVaToCpuVa(s_CurrentDataGpuVa, NULL);
        vm_offset_t s_CurrentData2CpuVa = s_CurrentData2GpuVa ? SceSblDriverGpuVaToCpuVa(s_CurrentData2GpuVa, NULL) : 0;

        if (s_SegmentDataCpuVa && s_CurrentDataCpuVa) {
            if (s_CurrentData2GpuVa && s_CurrentData2GpuVa != s_CurrentDataGpuVa && s_DataOffset > 0) {

                /* data spans two consecutive memory's pages, so we need to copy twice */
                uint32_t s_Size = PAGE_SIZE - s_DataOffset;
                memcpy((char*)s_SegmentDataCpuVa, (char*)s_CurrentDataCpuVa + s_DataOffset, s_Size);

                // prevent *potential* kpanic here
                if (s_CurrentData2CpuVa) {
                    memcpy((char *) s_SegmentDataCpuVa + s_Size, (char *) s_CurrentData2CpuVa, s_DataSize - s_Size);
                }
            } else {
                memcpy((char*)s_SegmentDataCpuVa, (char*)s_CurrentDataCpuVa + s_DataOffset, s_DataSize);
            }
        }

        /* setting error field to zero, thus we have no errors */
        *(int*)(pRequest + 0x04) = 0;

        return 0;
    }
}

FakeSelfHelper::FakeSelfHelper()
{
    auto sv = (struct sysentvec*)resolve(addr_sysvec);
    struct sysent* sysents = sv->sv_table;

    HookFunctionCall((uint8_t*)sysents[409].sy_call, (void*)SceSblAuthMgrVerifyHeaderHook, resolve(addr_sceSblAuthMgrVerifyHeaderHook1));
    HookFunctionCall((uint8_t*)sysents[384].sy_call, (void*)SceSblAuthMgrVerifyHeaderHook, resolve(addr_sceSblAuthMgrVerifyHeaderHook2));
    HookFunctionCall((uint8_t*)sysents[385].sy_call, (void*)SceSblAuthMgrIsLoadable2Hook, resolve(addr_SceSblAuthMgrIsLoadable2Hook));
    HookFunctionCall((uint8_t*)sysents[387].sy_call, (void*)SceSblAuthMgrSmLoadSelfSegment_MailboxHook, resolve(addr_SceSblAuthMgrSmLoadSelfSegment_Mailbox));
    HookFunctionCall((uint8_t*)sysents[386].sy_call, (void*)SceSblAuthMgrSmLoadSelfBlock_MailboxHook, resolve(addr_SceSblAuthMgrSmLoadSelfBlock_Mailbox));
}

FakeSelfHelper::~FakeSelfHelper()
{
    
}