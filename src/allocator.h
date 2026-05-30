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
    // 用于分配大于128字节的大块内存   malloc/free
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



    // 用于分配小于等于128字节的内存  内存池 + free_list  同时整合大于128字节内存的分配操作
    enum{ MAX_BYTES = 128};  // 内存上限
    enum{ ALIGN = 8};  // 8字节对齐
    enum{NOBJS = 20};  // 一次申请20小块

    // 字节数调整 按8的倍数调
    inline size_t ROUND_UP(size_t bytes)
    {
        return ((bytes + ALIGN - 1) & ~(ALIGN - 1));
    }

    // 计算free_list下标/索引
    inline size_t FREELIST_INDEX(size_t bytes)
    {
        return ROUND_UP(bytes)/ALIGN - 1;
    }

    // free_list节点
    union obj
    {
        union obj* next;
        char data[1];
    };

    // 整合分配类
    template <int inst>
    class __default_alloc_template
    {
    public:
        static void* allocate(size_t n)
        {
            if(n > MAX_BYTES)
                return malloc_alloc::allocate(n);
            
            size_t idx = FREELIST_INDEX(n);
            obj* list = free_list_[idx];

            if(!list)
                return refill(ROUND_UP(n));

            free_list_[idx] = list->next;
            return list;
        }

        static void deallocate(void* p, size_t n)
        {
            if(n > MAX_BYTES)
            {
                malloc_alloc::deallocate(p, n);
                return;
            }
            size_t idx = FREELIST_INDEX(n);
            obj* q = static_cast<obj*>(p);
            q ->next = free_list_[idx];
            free_list_[idx] = q;
        }

        static void* reallocate(void* p, size_t old_sz, size_t new_sz)
        {
            deallocate(p, old_sz);
            return allocate(new_sz);
        }
    private:
        // 从内存池中取一块---用于refill里面 已知start和end
        static char* chunk_alloc(size_t size, size_t& nobjs)
        {
            char* res = nullptr;
            size_t total  = size * nobjs;  // 所需内存
            size_t left = end_free_ - start_free_;  // 内存池剩余内存

            if(left >= total)
            {
                res = start_free_;
                start_free_ +=total;
                return res;
            }
            else if(left >= size)  // 内存池不够一整批，但是够一小块
            {
                nobjs = left/size;
                total = size*nobjs;
                res = start_free_;
                start_free_ += total;
            }
            else{
                size_t add = total * 2;
                if(left>0)
                {
                    size_t idx = FREELIST_INDEX(left);
                    reinterpret_cast<obj*>(start_free_)->next = free_list_[idx];
                    free_list_[idx] = reinterpret_cast<obj*>(start_free_);
                }
                start_free_ = static_cast<char*>(malloc_alloc::allocate(add));
                end_free_ = start_free_ + add;
                return chunk_alloc(size, nobjs);
            }
        }

        // 填充free_list---给空的自由链表free_list补货，
        // 当链表没内存块可用了，就调用它去内存池拿一大块，切成很多小块，串成链表，让下次分配能直接用。
        static void* refill(size_t size)  // size是一个块的大小
        {
            size_t nobjs = NOBJS;  // 需要NOBJS那么大
            char* chunk = chunk_alloc(size, nobjs);
            if(nobjs == 1) return chunk;  // 如果只能取出一块直接返回

            obj** my_free = free_list_ + FREELIST_INDEX(size);
            obj* res = reinterpret_cast<obj*>(chunk);
            *my_free = reinterpret_cast<obj*>(chunk + size);
            obj* cur = *my_free;

            for (size_t i = 1; i < nobjs - 1; ++i)
            {
                cur->next = reinterpret_cast<obj*>(
                    reinterpret_cast<char*>(cur) + size);
                cur = cur->next;
            }
            cur->next = nullptr;
            return res;
        }

        static obj* free_list_[MAX_BYTES / ALIGN];
        static char* start_free_;
        static char* end_free_;
    };

     // 初始化静态变量
     template <int inst>
    obj* __default_alloc_template<inst>::free_list_[MAX_BYTES / ALIGN] = {nullptr};
    template <int inst>
    char* __default_alloc_template<inst>::start_free_ = nullptr;
    template <int inst>
    char* __default_alloc_template<inst>::end_free_ = nullptr;
    using default_alloc = __default_alloc_template<0>;



    // 全局的构造和析构---在cpp中new/delete把内存与构造、析构绑死了；
    //但在STL中比如 vector一开始就申请 100 个 int 的内存，但现在还不想构造对象，等你 push_back 时才构造 ；
    //如果用 new一申请内存就自动构造 100 个对象，浪费、低效、不可控
    template <class T, class... Args>
    inline void construct(T* p, Args&&... args)
    {
        new (p) T(std::forward<Args>(args)...);
    }

    template <class T>
    inline void destroy(T* p)
    {
        p->~T();
    }

    template <class ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last)
    {
        using T = typename iterator_traits<ForwardIterator>::value_type;
        if constexpr (has_trivial_destructor_v<T>)
            return;
        for (; first != last; ++first)
            destroy(&*first);
    }



    // 对外的统一标准分配器
    template <class T>
    class allocator
    {
    public:
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        template <class U>
        struct rebind { using other = allocator<U>; };

        allocator() = default;
        allocator(const allocator&) = default;
        template <class U>
        allocator(const allocator<U>&) {}

        pointer allocate(size_type n)
        {
            if (n == 0) return nullptr;
            return static_cast<pointer>(default_alloc::allocate(n * sizeof(T)));
        }

        void deallocate(pointer p, size_type /*n*/)
        {
            default_alloc::deallocate(p, sizeof(T));
        }

        template <class... Args>
        void construct(pointer p, Args&&... args)
        {
            mytiny::construct(p, std::forward<Args>(args)...);
        }

        void destroy(pointer p)
        {
            mytiny::destroy(p);
        }

        size_type max_size() const { return size_type(-1) / sizeof(T); }
        pointer address(reference x) const { return &x; }
        const_pointer address(const_reference x) const { return &x; }
    };

    // 分配器无状态，全局共用一套内存池。
    template <class T, class U>
    bool operator==(const allocator<T>&, const allocator<U>&) { return true; }
    template <class T, class U>
    bool operator!=(const allocator<T>&, const allocator<U>&) { return false; }

}

#endif