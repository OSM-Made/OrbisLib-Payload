#include <stddef.h>
#include <stdint.h>

#include "sections.h"
#include "sparse.h"
#include "offsets.h"
#include "freebsd_helper.h"
#include "elf_helper.h"
#include "self_helper.h"
#include "sbl_helper.h"
#include "amd_helper.h"

#define PAGE_SIZE 0x4000

extern void* (*malloc)(unsigned long size, void* type, int flags) PAYLOAD_BSS;
extern void (*free)(void* addr, void* type) PAYLOAD_BSS;
extern void* (*memcpy)(void* dst, const void* src, size_t len) PAYLOAD_BSS;
extern size_t (*strlen)(const char *str) PAYLOAD_BSS;
extern char * (*strstr) (const char *haystack, const char *needle) PAYLOAD_BSS;

extern void* M_TEMP PAYLOAD_BSS;
extern struct sbl_map_list_entry** sbl_driver_mapped_pages PAYLOAD_BSS;
extern uint8_t* mini_syscore_self_binary PAYLOAD_BSS;

extern int (*sceSblServiceMailbox)(unsigned long service_id, uint8_t request[SBL_MSG_SERVICE_MAILBOX_MAX_SIZE], void* response) PAYLOAD_BSS;
extern int (*sceSblAuthMgrGetSelfInfo)(struct self_context* ctx, struct self_ex_info** info) PAYLOAD_BSS;
extern void (*sceSblAuthMgrSmStart)(void**) PAYLOAD_BSS;
extern int (*sceSblAuthMgrIsLoadable2)(struct self_context* ctx, struct self_auth_info* old_auth_info, int path_id, struct self_auth_info* new_auth_info) PAYLOAD_BSS;
extern int (*sceSblAuthMgrVerifyHeader)(struct self_context* ctx) PAYLOAD_BSS;
extern int (*sceSblACMgrGetPathId) (const char* path) PAYLOAD_BSS;

static const uint8_t s_auth_info_for_exec[] PAYLOAD_RDATA =
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

static const uint8_t s_auth_info_for_dynlib[] PAYLOAD_RDATA =
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

PAYLOAD_CODE static inline void* alloc(uint32_t size)
{
  return malloc(size, M_TEMP, 2);
}

PAYLOAD_CODE static inline void dealloc(void* addr)
{
  free(addr, M_TEMP);
}

PAYLOAD_CODE static struct sbl_map_list_entry* sceSblDriverFindMappedPageListByGpuVa(vm_offset_t gpu_va)
{
  struct sbl_map_list_entry* entry;
  if (!gpu_va)
  {
    return NULL;
  }
  entry = *sbl_driver_mapped_pages;
  while (entry)
  {
    if (entry->gpu_va == gpu_va)
    {
      return entry;
    }
    entry = entry->next;
  }
  return NULL;
}

PAYLOAD_CODE static inline vm_offset_t sceSblDriverGpuVaToCpuVa(vm_offset_t gpu_va, size_t* num_page_groups)
{
  struct sbl_map_list_entry* entry = sceSblDriverFindMappedPageListByGpuVa(gpu_va);
  if (!entry)
  {
    return 0;
  }
  if (num_page_groups)
  {
    *num_page_groups = entry->num_page_groups;
  }
  return entry->cpu_va;
}

PAYLOAD_CODE static inline int sceSblAuthMgrGetSelfAuthInfoFake(struct self_context* ctx, struct self_auth_info* info)
{
  struct self_header* hdr;
  struct self_fake_auth_info* fake_info;

  if (ctx->format == SELF_FORMAT_SELF)
  {
    hdr = (struct self_header*)ctx->header;
    fake_info = (struct self_fake_auth_info*)(ctx->header + hdr->header_size + hdr->meta_size - 0x100);
    if (fake_info->size == sizeof(fake_info->info))
    {
      memcpy(info, &fake_info->info, sizeof(*info));
      return 0;
    }
    return -37;
  }
  else
  {
    return -35;
  }
}

PAYLOAD_CODE static inline int is_fake_self(struct self_context* ctx)
{
  struct self_ex_info* ex_info;
  if (ctx && ctx->format == SELF_FORMAT_SELF)
  {
    if (sceSblAuthMgrGetSelfInfo(ctx, &ex_info))
    {
      return 0;
    }
    return ex_info->ptype == SELF_PTYPE_FAKE;
  }
  return 0;
}

PAYLOAD_CODE static inline int sceSblAuthMgrGetElfHeader(struct self_context* ctx, struct elf64_ehdr** ehdr)
{
  struct self_header* self_hdr;
  struct elf64_ehdr* elf_hdr;
  size_t pdata_size;

  if (ctx->format == SELF_FORMAT_ELF)
  {
    elf_hdr = (struct elf64_ehdr*)ctx->header;
    if (ehdr)
    {
      *ehdr = elf_hdr;
    }
    return 0;
  }
  else if (ctx->format == SELF_FORMAT_SELF)
  {
    self_hdr = (struct self_header*)ctx->header;
    pdata_size = self_hdr->header_size - sizeof(struct self_entry) * self_hdr->num_entries - sizeof(struct self_header);
    if (pdata_size >= sizeof(struct elf64_ehdr) && (pdata_size & 0xF) == 0)
    {
      elf_hdr = (struct elf64_ehdr*)((uint8_t*)self_hdr + sizeof(struct self_header) + sizeof(struct self_entry) * self_hdr->num_entries);
      if (ehdr)
      {
        *ehdr = elf_hdr;
      }
      return 0;
    }
    return -37;
  }
  return -35;
}

PAYLOAD_CODE static inline int build_self_auth_info_fake(struct self_context* ctx, struct self_auth_info* parent_auth_info, struct self_auth_info* auth_info)
{
  struct self_auth_info fake_auth_info;
  struct self_ex_info* ex_info;
  struct elf64_ehdr* ehdr = NULL;
  int result;

  if (!ctx || !parent_auth_info || !auth_info)
  {
    result = EINVAL;
    goto error;
  }

  if (!is_fake_self(ctx))
  {
    result = EINVAL;
    goto error;
  }

  result = sceSblAuthMgrGetSelfInfo(ctx, &ex_info);
  if (result)
  {
    goto error;
  }

  result = sceSblAuthMgrGetElfHeader(ctx, &ehdr);
  if (result)
  {
    goto error;
  }

  if (!ehdr)
  {
    result = ESRCH;
    goto error;
  }

  result = sceSblAuthMgrGetSelfAuthInfoFake(ctx, &fake_auth_info);
  if (result)
  {
    switch (ehdr->type)
    {
      case ELF_ET_EXEC:
      case ELF_ET_SCE_EXEC:
      case ELF_ET_SCE_EXEC_ASLR:
      {
        memcpy(&fake_auth_info, s_auth_info_for_exec, sizeof(fake_auth_info));
        result = 0;
        break;
      }

      case ELF_ET_SCE_DYNAMIC:
      {
        memcpy(&fake_auth_info, s_auth_info_for_dynlib, sizeof(fake_auth_info));
        result = 0;
        break;
      }

      default:
      {
        result = ENOTSUP;
        goto error;
      }
    }

    fake_auth_info.paid = ex_info->paid;

    // TODO: overwrite low bits of PAID with title id number
  }

  if (auth_info)
  {
    memcpy(auth_info, &fake_auth_info, sizeof(*auth_info));
  }

error:
  return result;
}

PAYLOAD_CODE static inline int auth_self_header(struct self_context* ctx)
{
  struct self_header* hdr;
  unsigned int old_total_header_size, new_total_header_size;
  int old_format;
  uint8_t* tmp;
  int is_unsigned;
  int result;

  is_unsigned = ctx->format == SELF_FORMAT_ELF || is_fake_self(ctx);
  if (is_unsigned)
  {
    old_format = ctx->format;
    old_total_header_size = ctx->total_header_size;

    /* take a header from mini-syscore.elf */
    hdr = (struct self_header*)mini_syscore_self_binary;

    new_total_header_size = hdr->header_size + hdr->meta_size;

    tmp = (uint8_t*)alloc(new_total_header_size);
    if (!tmp)
    {
      result = ENOMEM;
      goto error;
    }

    /* temporarily swap an our header with a header from a real SELF file */
    memcpy(tmp, ctx->header, new_total_header_size);
    memcpy(ctx->header, hdr, new_total_header_size);

    /* it's now SELF, not ELF or whatever... */
    ctx->format = SELF_FORMAT_SELF;
    ctx->total_header_size = new_total_header_size;

    /* call the original method using a real SELF file */
    result = sceSblAuthMgrVerifyHeader(ctx);

    /* restore everything we did before */
    memcpy(ctx->header, tmp, new_total_header_size);
    ctx->format = old_format;
    ctx->total_header_size = old_total_header_size;

    dealloc(tmp);
  }
  else
  {
    result = sceSblAuthMgrVerifyHeader(ctx);
  }

error:
  return result;
}

PAYLOAD_CODE int my_sceSblAuthMgrIsLoadable2(struct self_context* ctx, struct self_auth_info* old_auth_info, int path_id, struct self_auth_info* new_auth_info)
{
  if (ctx->format == SELF_FORMAT_ELF || is_fake_self(ctx))
  {
    return build_self_auth_info_fake(ctx, old_auth_info, new_auth_info);
  }
  else
  {
    return sceSblAuthMgrIsLoadable2(ctx, old_auth_info, path_id, new_auth_info);
  }
}

PAYLOAD_CODE int my_sceSblAuthMgrVerifyHeader(struct self_context* ctx)
{
  void* dummy;
  sceSblAuthMgrSmStart(&dummy);
  return auth_self_header(ctx);
}

PAYLOAD_CODE int my_sceSblAuthMgrSmLoadSelfSegment__sceSblServiceMailbox(unsigned long service_id, uint8_t* request, void* response)
{
  register struct self_context* ctx __asm ("r14"); // 5.05
  int is_unsigned = ctx && is_fake_self(ctx);

  if (is_unsigned)
  {
    *(int*)(response + 0x04) = 0; /* setting error field to zero, thus we have no errors */
    return 0;
  }
  return sceSblServiceMailbox(service_id, request, response);
}

PAYLOAD_CODE int my_sceSblAuthMgrSmLoadSelfBlock__sceSblServiceMailbox(unsigned long service_id, uint8_t* request, void* response)
{
  uint8_t* frame = (uint8_t*)__builtin_frame_address(1);
  struct self_context* ctx = *(struct self_context**)(frame - 0x1C8); // 5.05
  vm_offset_t segment_data_gpu_va = *(unsigned long*)(request + 0x08);
  vm_offset_t cur_data_gpu_va = *(unsigned long*)(request + 0x50);
  vm_offset_t cur_data2_gpu_va = *(unsigned long*)(request + 0x58);
  unsigned int data_offset = *(unsigned int*)(request + 0x44);
  unsigned int data_size = *(unsigned int*)(request + 0x48);
  vm_offset_t segment_data_cpu_va, cur_data_cpu_va, cur_data2_cpu_va;
  unsigned int size1;

  int is_unsigned = ctx && (ctx->format == SELF_FORMAT_ELF || is_fake_self(ctx));
  int result;

  if (is_unsigned)
  {
    /* looking into lists of GPU's mapped memory regions */
    segment_data_cpu_va = sceSblDriverGpuVaToCpuVa(segment_data_gpu_va, NULL);
    cur_data_cpu_va = sceSblDriverGpuVaToCpuVa(cur_data_gpu_va, NULL);
    cur_data2_cpu_va = cur_data2_gpu_va ? sceSblDriverGpuVaToCpuVa(cur_data2_gpu_va, NULL) : 0;

    if (segment_data_cpu_va && cur_data_cpu_va)
    {
      if (cur_data2_gpu_va && cur_data2_gpu_va != cur_data_gpu_va && data_offset > 0)
      {
        /* data spans two consecutive memory's pages, so we need to copy twice */
        size1 = PAGE_SIZE - data_offset;
        memcpy((char*)segment_data_cpu_va, (char*)cur_data_cpu_va + data_offset, size1);
        memcpy((char*)segment_data_cpu_va + size1, (char*)cur_data2_cpu_va, data_size - size1);
      }
      else
      {
        memcpy((char*)segment_data_cpu_va, (char*)cur_data_cpu_va + data_offset, data_size);
      }
    }

    *(int*)(request + 0x04) = 0; /* setting error field to zero, thus we have no errors */
    result = 0;
  }
  else
  {
    result = sceSblServiceMailbox(service_id, request, response);
  }

  return result;
}

PAYLOAD_CODE int my_sceSblAuthMgrIsLoadable__sceSblACMgrGetPathId(const char* path) {
	static const char* self_dir_prefix = "/data/self/";
	const char* p;
	int ret;

	if (path) {
		p = strstr(path, self_dir_prefix);
		if (p)
			path = p + strlen(self_dir_prefix);
	}

	ret = sceSblACMgrGetPathId(path);

	return ret;
}

PAYLOAD_CODE void install_fself_hooks()
{
  uint64_t flags, cr0;
  uint64_t kernbase = getkernbase();

  cr0 = readCr0();
  writeCr0(cr0 & ~X86_CR0_WP);
  flags = intr_disable();

  KCALL_REL32(kernbase, sceSblAuthMgrIsLoadable2_hook, (uint64_t)my_sceSblAuthMgrIsLoadable2);
  KCALL_REL32(kernbase, sceSblAuthMgrVerifyHeader_hook1, (uint64_t)my_sceSblAuthMgrVerifyHeader);
  KCALL_REL32(kernbase, sceSblAuthMgrVerifyHeader_hook2, (uint64_t)my_sceSblAuthMgrVerifyHeader);
  KCALL_REL32(kernbase, sceSblAuthMgrSmLoadSelfSegment__sceSblServiceMailbox_hook, (uint64_t)my_sceSblAuthMgrSmLoadSelfSegment__sceSblServiceMailbox);
  KCALL_REL32(kernbase, sceSblAuthMgrSmLoadSelfBlock__sceSblServiceMailbox_hook, (uint64_t)my_sceSblAuthMgrSmLoadSelfBlock__sceSblServiceMailbox);
  KCALL_REL32(kernbase, sceSblAuthMgrIsLoadable__sceSblACMgrGetPathId__hook, (uint64_t)my_sceSblAuthMgrIsLoadable__sceSblACMgrGetPathId);

  intr_restore(flags);
  writeCr0(cr0);
}
