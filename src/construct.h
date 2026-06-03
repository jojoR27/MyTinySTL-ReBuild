// 默认构造和析构函数

#ifndef MYTINYSTL_CONSTRUCT_H_
# define MYTINYSTL_CONSTRUCT_H_

#include "type_traits.h"
#include "iterator.h"
#include <utility>

namespace mytiny{

    // 构造
    template <class T, class... Args>
    inline void construct(T* ptr, Args&&... args)
    {
        new (ptr) T(std::forward<Args>(args)...);
    }

    // 析构
    template <class T>
    inline void destory(T* ptr)
    {
        ptr->~T();
    }

    // 批量销毁
    template <class ForwardIter>
    inline void destory(ForwardIter first, ForwardIter last)
    {
        using ValT = typename iterator_traits<ForwardIter>::value_type;
        // POD类型没有自定义析构
        using IsTrivial = typename type_traits<ValT>::has_trivial_dtor;
        _destroy(first, last, IsTrivial());
    }

    // opd类型的平凡析构
    template <class ForwardIter>
    inline void _destroy(ForwardIter first, ForwardIter last, _false_type)
    {
        for(; first != last; first++){
            destroy(&*first);
        }
    }

    // 非opd类型的非平凡析构
    template <class ForwardIter>
    inline void _destroy(ForwardIter first, ForwardIter last, _false_type)
    {
    }
}

# endif