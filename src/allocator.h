// 这个文件实现了 STL内存分配器：负责 “申请内存、释放内存、构造对象、析构对象”

#ifndef MYTINYSTL_ALLOCATOR_H_
#define MYTINYSTL_ALLOCATOR_H_

#include <cstddef>
#include <cstdlib>
#include <climits>
#include "type_traits.h"
#include "iterator.h"

namespace mytiny
{
    // 用于分配大于128字节的大块内存
    template <int inst>
    class __malloc_alloc_template
    {
        public:
            // 申请n字节内存
            static void* allocate(size_t n)
            {
                void *res = std::malloc(n);
                // 内存不足则调用oom函数
                if(!res) res = oom_malloc(n);
                return res;
            }

            // 释放内存  后面一个参数没用 但是接口需要
            static void deallocate(void* p, size_t /*n*/)
            {
                std::free(p);
            }

            // 重新分配内存
            static void* reallocate(void* p, size_t /*old_sz*/, size_t new_sz)
            {
                void* res = std::realloc(p, new_sz)
                if(!res) res = oom_malloc(new_sz);
                return res;
            }

            // 内存不足时的 回调函数指针类型
            using malloc_handler = void (*)();

            // 设置回调函数 
            static malloc_handler set_malloc_handler(malloc_handler f)
            {
                malloc_handler old = handler_;
                handler_ = f;
                return old;
            }
        private:
            // 处理内存不足
            static void* oom_malloc(size_t n)
            {
                // h是回调函数指针 回调函数里面会释放内存
                malloc_handler h = handler_;
                while(h)
                {
                    h();
                    void* res = std::malloc(n);
                    if(res) return res;
                    h = handler_;
                }
                return nullptr;
            }

            static malloc_handler handler_;
    };

    // 初始化静态变量
    template <int inst>
    typename __malloc_alloc_template<inst>::malloc_handler  // typename __malloc_alloc_template<inst>::malloc_handler变量类型， __malloc_alloc_template<inst>::handler_变量名
    __malloc_alloc_template<inst>::handler_ = nullptr;

    using __malloc_alloc = __malloc_alloc_template<0>;



    // 用于分配小于等于128字节的大块内存
    enum{ MAX_BYTES = 128};  // 内存上限
    enum{ ALIGN = 8};  // 8字节对齐
    enum{NOBJS = 20};  // 一次申请20小块

    // 字节数调整 按8的倍数调
    inline size_t ROUND_UP(size_t bytes)
    {
        return ((bytes + ALIGN - 1) & ~(ALIGN - 1));
    }

    // 计算free_list下标
    inline size_t FREELIST_INDEX(size_t bytes)
    {
        return ROUND_UP(bytes)/ALIGN - 1;
    }


    
    union obj
    {
        union obj* next;
        char data[1];
    };


}


#endif