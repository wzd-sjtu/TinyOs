#ifndef __KERNEL_MEMORY_H
#define __KERNEL_MEMORY_H
#include "stdint.h"
#include "bitmap.h"
#include "list.h"

/* 内存池标记,用于判断用哪个内存池 */
enum pool_flags {
   PF_KERNEL = 1,    // 内核内存池
   PF_USER = 2	     // 用户内存池
};

#define	 PG_P_1	  1	// 页表项或页目录项存在属性位
#define	 PG_P_0	  0	// 页表项或页目录项存在属性位
#define	 PG_RW_R  0	// R/W 属性位值, 读/执行
#define	 PG_RW_W  2	// R/W 属性位值, 读/写/执行
#define	 PG_US_S  0	// U/S 属性位值, 系统级
#define	 PG_US_U  4	// U/S 属性位值, 用户级

#define DESC_CNT 7 // mem_block_desc's number

/* 用于虚拟地址管理 */
struct virtual_addr {
/* 虚拟地址用到的位图结构，用于记录哪些虚拟地址被占用了。以页为单位。*/
   struct bitmap vaddr_bitmap;
/* 管理的虚拟地址 */
   uint32_t vaddr_start;
};


struct mem_block {
   struct list_elem free_elem; // every mem_block is a list_elem
};
struct mem_block_desc {
   uint32_t block_size;
   uint32_t blocks_per_arena;
   struct list free_list; // free memory storage
};


extern struct pool kernel_pool, user_pool;
void mem_init(void);
void* get_kernel_pages(uint32_t pg_cnt);
void* malloc_page(enum pool_flags pf, uint32_t pg_cnt);
void malloc_init(void);
uint32_t* pte_ptr(uint32_t vaddr);
uint32_t* pde_ptr(uint32_t vaddr);
uint32_t addr_v2p(uint32_t vaddr);
void* get_a_page(enum pool_flags pf, uint32_t vaddr);
void* get_user_pages(uint32_t pg_cnt);
void block_desc_init(struct mem_block_desc* desc_array);

void* sys_malloc(uint32_t size);
void sys_free(void* ptr);

#endif


/* 内存池结构,生成两个实例用于管理内核内存池和用户内存池
struct pool {
   struct bitmap pool_bitmap;	 // 本内存池用到的位图结构,用于管理物理内存
   uint32_t phy_addr_start;	 // 本内存池所管理物理内存的起始地址
   uint32_t pool_size;		 // 本内存池字节容量
   struct lock lock;		 // 申请内存时互斥
};

struct arena {
   struct mem_block_desc* desc; // description
   uint32_t cnt;
   int large; // whether it is bigger than 2048bytes?
};
*/