#include "../Main.hpp"
#include "Proc.hpp"

proc* GetCurrentGame()
{
    proc *allproc = *(proc**)resolve(addr_allproc);
    char TitleID[10];
    strcpy(TitleID, "N/A");

    while (allproc != NULL)
    {
		Log("%s", allproc->titleId);
		if(!strcmp(allproc->p_comm, "eboot.bin") || !strcmp(allproc->p_comm, "default.elf") ||!strcmp(allproc->p_comm, "default_mp.elf"))
		{
			char TitlePart[4];
			memcpy(TitlePart, allproc->titleId, 4);

			if(!strcmp(allproc->titleId, "NPSX"))
				continue;

			Log("%s", allproc->titleId);
			break;
		}

        allproc = allproc->p_list.le_next;
    }

    return allproc;
}

int get_proc_count() {
	int count = 0;
	proc *p = *(proc **)resolve(addr_allproc);

	do {
		count++;
	} while ((p = p->p_list.le_next));

	return count;
}

proc *proc_find_by_name(const char *name) 
{
	if (!name) {
		return NULL;
	}

	proc *p = *(proc **)resolve(addr_allproc);

	do {
		if (!memcmp(p->p_comm, name, strlen(name))) {
			return p;
		}
	} while ((p = p->p_list.le_next));

	return NULL;
}

proc *proc_find_by_pid(int pid) {
	proc *p = *(proc **)resolve(addr_allproc);

	do {
		if (p->p_pid == pid) {
			return p;
		}
	} while ((p = p->p_list.le_next));

	return NULL;
}

int proc_get_vm_map(proc *p, proc_vm_map_entry **entries, size_t *num_entries) {
	proc_vm_map_entry *info = NULL;
	vm_map_entry *entry = NULL;

	vmspace *vm = vmspace_acquire_ref(p);
	if (!vm) {
		return 1;
	}

	vm_map *map = &vm->vm_map;

	int num = map->nentries;
	if (!num) {
		vmspace_free(vm);
		return 0;
	}

	vm_map_lock_read(map);

	if (vm_map_lookup_entry(map, NULL, &entry)) {
		vm_map_unlock_read(map);
		vmspace_free(vm);
		return 1;
	}

	info = (proc_vm_map_entry *)_malloc(num * sizeof(proc_vm_map_entry));
	if (!info) {
		vm_map_unlock_read(map);
		vmspace_free(vm);
		return 1;
	}

	for (int i = 0; i < num; i++) {
		info[i].start = entry->start;
		info[i].end = entry->end;
		info[i].offset = entry->offset;
		info[i].prot = entry->protection & (entry->protection >> 8);
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

int proc_rw_mem(proc *p, void *ptr, size_t size, void *data, size_t *n, int write) {
	thread *td = curthread();
	iovec iov;
	uio uio;
	int r = 0;

	if (!p) {
		return 1;
	}

	if (size == 0) {
		if (n) {
			*n = 0;
		}

		return 0;
	}

	memset(&iov, NULL, sizeof(iov));
	iov.iov_base = (void*)data;
	iov.iov_len = size;

	memset(&uio, NULL, sizeof(uio));
	uio.uio_iov = &iov;
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

int proc_read_mem(struct proc *p, void *ptr, size_t size, void *data, size_t *n) 
{
	return proc_rw_mem(p, ptr, size, data, n, 0);
}

int proc_write_mem(struct proc *p, void *ptr, size_t size, void *data, size_t *n) 
{
	return proc_rw_mem(p, ptr, size, data, n, 1);
}

int proc_allocate(struct proc *p, void **address, size_t size) {
	uint64_t addr = NULL;
	int r = 0;

	if (!address) 
        return 1;

	vmspace *vm = p->p_vmspace;
	vm_map *map = &vm->vm_map;

	vm_map_lock(map);

	r = vm_map_findspace(map, NULL, size, &addr);
	if (r) {
		vm_map_unlock(map);
		return r;
	}

	r = vm_map_insert(map, NULL, NULL, addr, addr + size, VM_PROT_ALL, VM_PROT_ALL, 0);

	vm_map_unlock(map);

	if (r)
        return r;

	if (address) {
		*address = (void *)addr;
	}

	return r;
}

int proc_deallocate(proc *p, void *address, size_t size) {
	int r = 0;

	vmspace *vm = p->p_vmspace;
	vm_map *map = &vm->vm_map;

	vm_map_lock(map);

	r = vm_map_delete(map, (uint64_t)address, (uint64_t)address + size);

	vm_map_unlock(map);

	return r;
}

int proc_mprotect(proc *p, void *address, void *end, int new_prot) {
	int r = 0;

	uint64_t addr = (uint64_t)address;
	uint64_t addrend = (uint64_t)end;

	vmspace *vm = p->p_vmspace;
	vm_map *map = &vm->vm_map;

	r = vm_map_protect(map, addr, addrend, new_prot, 1);
	r = vm_map_protect(map, addr, addrend, new_prot, 0);
	
	return r;
}


uint64_t proc_alloc_size(uint64_t p)
{
	uint64_t ldrsize = p;
	ldrsize += (PAGE_SIZE - (ldrsize % PAGE_SIZE));
	return ldrsize;
}