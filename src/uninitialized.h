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


    // 容器“初始化”函数，eg:vector<int>v1(10, 9)
    // 未初始化空间上填充10个9  填充n个value
    // POD类型
    template <class ForwardIter, class Size, class T>
    ForwardIter
    unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, _true_type)
    {
        for(Size i = 0; i<n; ++i,++first)
        {
            *first = value;
        }
        return first;
    }

    // 非POD类型
    template <class ForwardIter, class Size, class T>
    ForwardIter
    unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, _false_type)
    {
        ForwardIter cur = first;
        try
        {
            {
                for(Size i =0; i<n; ++i, ++cur)
                {
                    construct(&*cur, value);
                }
            }
        }
        catch(...)
        {
            destory(first, cur);
            throw;
        }
        return cur;
    }

    // 统一接口
    template <class ForwardIter, class Size, class T>
    ForwardIter
    unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value)
    {
        using ValType = typename iterator_traits<ForwardIter>::value_type;
        using isPOD = typename type_traits<ValType>::is_POD_type;
        return unchecked_uninit_fill_n(first, n, value, isPOD());
    }


    // 未初始化区间[first,last)填充value
    // POD类型
    template <class ForwardIter, class T>
    void 
    unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T& value, _true_type)
    {
        for (; first != last; ++first)
        {
            *first = value;
        }
    }

    // 非POD类型
    template <class ForwardIter, class T>
    void 
    unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T& value, _false_type)
    {
        ForwardIter cur = first;
        try
        {
            for (; cur != last; ++cur)
            {
                construct(&*cur, value);
            }
        }
        catch (...)
        {
            destroy(first, cur);
            throw;
        }
    }

    // 统一接口
    template <class ForwardIter, class T>
    void 
    uninitialized_fill(ForwardIter first, ForwardIter last, const T& value)
    {
        using ValType = typename iterator_traits<ForwardIter>::value_type;
        using isPOD = typename type_traits<ValType>::is_POD_type;
        unchecked_uninit_fill(first, last, value, isPOD());
    }
}

#endif