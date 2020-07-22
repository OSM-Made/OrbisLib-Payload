#include <stddef.h>
#include <stdint.h>

#include "sections.h"
#include "sparse.h"
#include "offsets.h"
#include "freebsd_helper.h"

extern int (*proc_rwmem)(struct proc *p, struct uio *uio) PAYLOAD_BSS;
extern struct vmspace *(*vmspace_acquire_ref)(struct proc *p) PAYLOAD_BSS;
extern void (*vmspace_free)(struct vmspace *vm) PAYLOAD_BSS;
extern void (*vm_map_lock_read)(struct vm_map *map) PAYLOAD_BSS;
extern void (*vm_map_unlock_read)(struct vm_map *map) PAYLOAD_BSS;
extern int (*vm_map_lookup_entry)(struct vm_map *map, uint64_t address, struct vm_map_entry **entries) PAYLOAD_BSS;

extern size_t (*strlen)(const char *str) PAYLOAD_BSS;
extern void* (*malloc)(unsigned long size, void* type, int flags) PAYLOAD_BSS;
extern void (*free)(void* addr, void* type) PAYLOAD_BSS;
extern void* (*memcpy)(void* dst, const void* src, size_t len) PAYLOAD_BSS;
extern void* (*memset)(void *s, int c, size_t n) PAYLOAD_BSS;
extern int (*memcmp)(const void *ptr1, const void *ptr2, size_t num) PAYLOAD_BSS;

extern void* M_TEMP PAYLOAD_BSS;
extern struct proc** allproc PAYLOAD_BSS;

PAYLOAD_CODE static inline void *alloc(uint32_t size)
{
  return malloc(size, M_TEMP, 2);
}

PAYLOAD_CODE static inline void dealloc(void* addr)
{
  free(addr, M_TEMP);
}

PAYLOAD_CODE static struct proc *proc_find_by_name(const char *name)
{
  struct proc *p;

  if (!name) {
    return NULL;
  }

  p = *allproc;

  do {
    if (!memcmp(p->p_comm, name, strlen(name))) {
      return p;
    }
  } while ((p = p->p_forw));

  return NULL;
}

PAYLOAD_CODE static int proc_get_vm_map(struct proc *p, struct proc_vm_map_entry **entries, size_t *num_entries)
{
  struct proc_vm_map_entry *info = NULL;
  struct vm_map_entry *entry = NULL;

  struct vmspace *vm = vmspace_acquire_ref(p);
  if (!vm) {
    return -1;
  }

  struct vm_map *map = &vm->vm_map;

  int num = map->nentries;
  if (!num) {
    vmspace_free(vm);
    return 0;
  }

  vm_map_lock_read(map);

  if (vm_map_lookup_entry(map, 0, &entry)) {
    vm_map_unlock_read(map);
    vmspace_free(vm);
    return -1;
  }

  info = (struct proc_vm_map_entry *)alloc(num * sizeof(struct proc_vm_map_entry));
  if (!info) {
    vm_map_unlock_read(map);
    vmspace_free(vm);
    return -1;
  }

  for (int i = 0; i < num; i++) {
    info[i].start = entry->start;
    info[i].end = entry->end;
    info[i].offset = entry->offset;
    info[i].prot = entry->prot & (entry->prot >> 8);
    memcpy(info[i].name, entry->name, sizeof(info[i].name));

    if (!(entry = entry->next)) {
      break;
    }
  }

  vm_map_unlock_read(map);
  vmspace_free(vm);

  if (entries) {
    *entries = info;
  }

  if (num_entries) {
    *num_entries = num;
  }

  return 0;
}

PAYLOAD_CODE static int proc_rw_mem(struct proc *p, void *ptr, size_t size, void *data, size_t *n, int write)
{
  struct thread *td = curthread();
  struct iovec iov;
  struct uio uio;
  int r = 0;

  if (!p) {
    return -1;
  }

  if (size == 0) {
    if (n) {
      *n = 0;
    }

    return 0;
  }

  memset(&iov, 0, sizeof(iov));
  iov.iov_base = (uint64_t)data;
  iov.iov_len = size;

  memset(&uio, 0, sizeof(uio));
  uio.uio_iov = (uint64_t)&iov;
  uio.uio_iovcnt = 1;
  uio.uio_offset = (uint64_t)ptr;
  uio.uio_resid = (uint64_t)size;
  uio.uio_segflg = UIO_SYSSPACE;
  uio.uio_rw = write ? UIO_WRITE : UIO_READ;
  uio.uio_td = td;

  r = proc_rwmem(p, &uio);

  if (n) {
    *n = (size_t)((uint64_t)size - uio.uio_resid);
  }

  return r;
}

PAYLOAD_CODE static inline int proc_write_mem(struct proc *p, void *ptr, size_t size, void *data, size_t *n) {
  return proc_rw_mem(p, ptr, size, data, n, 1);
}

PAYLOAD_CODE int shellcore_fpkg_patch(void)
{
  uint8_t *text_seg_base = NULL;
  size_t n;

  struct proc_vm_map_entry *entries = NULL;
  size_t num_entries = 0;

  int ret = 0;

  uint32_t call_ofs_for__xor__eax_eax__3nop[] = {
    // call sceKernelIsGenuineCEX
    sceKernelIsGenuineCEX_patch1, 
    sceKernelIsGenuineCEX_patch2,
    sceKernelIsGenuineCEX_patch3,
    sceKernelIsGenuineCEX_patch4,
    // call nidf_libSceDipsw
    nidf_libSceDipsw_patch1,
    nidf_libSceDipsw_patch2,
    nidf_libSceDipsw_patch3,
    nidf_libSceDipsw_patch4,
  };

  uint8_t xor__eax_eax__inc__eax[5] = { 0x31, 0xC0, 0xFF, 0xC0, 0x90 };

  struct proc *ssc = proc_find_by_name("SceShellCore");

  if (!ssc) {
    ret = -1;
    goto error;
  }

  ret = proc_get_vm_map(ssc, &entries, &num_entries);
  if (ret) {
    goto error;
  }

  for (int i = 0; i < num_entries; i++) {
    if (entries[i].prot == (PROT_READ | PROT_EXEC)) {
      text_seg_base = (uint8_t *)entries[i].start;
      break;
    }
  }

  if (!text_seg_base) {
    ret = -1;
    goto error;
  }

  // enable installing of debug packages
  for (int i = 0; i < COUNT_OF(call_ofs_for__xor__eax_eax__3nop); i++) {
    ret = proc_write_mem(ssc, (void *)(text_seg_base + call_ofs_for__xor__eax_eax__3nop[i]), 5, "\x31\xC0\x90\x90\x90", &n);
    if (ret) {
      goto error;
    }
  }

  ret = proc_write_mem(ssc, text_seg_base + SHELLCORE_SANDBOX_ENABLE_DATA_MOUNT_OFFSET, sizeof(xor__eax_eax__inc__eax), xor__eax_eax__inc__eax, &n);
  if (ret) {
      goto error;
    }

  // enable fpkg for patches
  ret = proc_write_mem(ssc, (void *)(text_seg_base + enable_fpkg_patch), 8, "\xE9\x96\x00\x00\x00\x90\x90\x90", &n);
  if (ret) {
    goto error;
  }

  // this offset corresponds to "fake\0" string in the Shellcore's memory
  ret = proc_write_mem(ssc, (void *)(text_seg_base + fake_free_patch), 5, "free\0", &n);
  if (ret) {
    goto error;
  }

  // make pkgs installer working with external hdd
  ret = proc_write_mem(ssc, (void *)(text_seg_base + pkg_installer_patch), 1, "\0", &n);
  if (ret) {
    goto error;
  }

error:
  if (entries) {
    dealloc(entries);
  }

  return ret;
}
