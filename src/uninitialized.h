// 在还没有构造对象的空白内存上，批量创建对象。
//例如vector容器创建时不初始化但是有空白内存

#ifndef MYTINYSTL_UNINITIALIZED_H_
#define MYTINYSTL_UNINITIALIZED_H_
#include "type_traits"
#include "iterator.h"
#include "construct.h"
#include <cstring>

namespace mytiny
{
    // POD类型 把未初始化空间的first->last 拷贝到result
    // POD 只是纯数据，不用构造，不用析构，直接拷贝即可。
    template <class InputIter, class ForwardIter>
    ForwardIter
    unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, _true_type)
    {
        memmove(result, first, sizeof(*first)*(last - first));
        return result + (last - first);
    }

    // 非POD类型
    // 非 POD 对象有资源、有构造、有析构，必须手动创建、手动销毁，否则泄漏 / 崩溃。
    template <class InputIter, class ForwardIter>
    ForwardIter
    unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, _false_type)
    {
        ForwardIter cur = result;
        try
        {   
            for(; first!=last;++first,++cur)
            {
                construct(&*cur, *first);
            }
        }
        catch(...)
        {
            destory(result, cur);
            throw;
        }
        return cur;
    }

    // 统一接口
    template <class InputIter, class ForwardIter>
    ForwardIter
    uninitialized_copy(InputIter first, InputIter last, ForwardIter result)
    {
        using ValType = typename iterator_traits<ForwardIter>::value_type;
        using isPOD  = typename type_traits<ValType>::is_POD_type;
        return unchecked_uninit_copy(first, last, result, isPOD());
    }
}

#endif