#pragma once

struct proc_vm_map_entry {
	char name[32];
	vm_offset_t start;
	vm_offset_t end;
	vm_offset_t offset;
	uint16_t prot;
};

uint64_t proc_alloc_size(uint64_t p);
int get_proc_count();
proc *proc_find_by_name(const char *name);
proc *proc_find_by_pid(int pid);
int proc_get_vm_map(proc *p, proc_vm_map_entry **entries, size_t *num_entries);

int proc_rw_mem(proc *p, void *ptr, size_t size, void *data, size_t *n, int write);
int proc_read_mem(proc *p, void *ptr, size_t size, void *data, size_t *n);
int proc_write_mem(proc *p, void *ptr, size_t size, void *data, size_t *n);
int proc_allocate(proc*p, void **address, size_t size);
int proc_deallocate(proc *p, void *address, size_t size);
int proc_mprotect(proc *p, void *address, void *end, int new_prot);