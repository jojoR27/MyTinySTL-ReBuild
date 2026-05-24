// 实现STL迭代器iterator的核心基础设施

#ifndef MYTINYSTL_ITERATOR_H_
#define MYTINYSTL_ITERATOR_H_
#include "type_traits.h"
#include <cstddef>

namespace mytiny
{
    // 迭代器类型标签---根据不同迭代器标签后续编译器调用不同高效算法
    struct input_iterator_tag {};  // 输入迭代器，只读
    struct output_iterator_tag {}; // 输出迭代器，只写
    struct forward_iterator_tag: public input_iterator_tag {};  // 前向迭代器：继承输入迭代器，多次向前读
    struct bidirectional_iterator_tag : public forward_iterator_tag {};  // 双向迭代器
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};  // 随机访问迭代器


    // 迭代器基础模板结构体---所有自定义迭代器都要继承该类
    template <
        class Category,  // 迭代器类型，对应上面五种
        class T,  // 迭代器指向的值/容器元素类型
        class Distance = ptrdiff_t,  // 迭代器距离
        class Pointer = T*,  // 某容器元素的指针
        class Reference = T&  // 某容器元素的引用
    >
    struct iterator{
        // 指明上面的数据类型，前两个是自定义数据类型
        typedef Category iterator_category;
        typedef T value_type;
        typedef Pointer pointer;
        typedef Reference reference;
        typedef Distance difference_type;
    }; 


    // 通用迭代器萃取结构体---算法通过它知道某个迭代器中的五个数据类型
    template <class Iterator>
    struct iterator_traits
    {
        typedef typename Iterator::iterator_category iterator_category;  // 从迭代器里取出typename Iterator::iterator_category这个类型 取名为 iterator_category
        typedef typename Iterator::value_type value_type;
        typedef typename Iterator::pointer pointer;
        typedef typename Iterator::reference reference;
        typedef typename Iterator::difference_type difference_type;
    };

    // 普通指针也是一个迭代器(随机访问)，做偏特化
    template<class T>
    struct iterator_traits<T*>
    {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;
        typedef ptrdiff_t difference_type;
    };
    
    // const指针偏特化
    template<class T>
    struct iterator_traits<const T*>
    {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef const T* pointer;
        typedef const T& reference;
        typedef ptrdiff_t difference_type;
    };


    // -------辅助工具函数---获取某个迭代器内部各个类型的类型标签
    // 获取迭代器类型标签对象
    //内联函数iterator_category()返回值数据类型是typename iterator_traits<Iterator>::iterator_category
    template <class Iterator>
    inline typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator&)
    {
        typedef typename iterator_traits<Iterator>::iterator_category category;
        return category();
    }
    // 获取距离类型指针
    template<class Iterator>
    inline typename iterator_traits<Iterator>::difference_type*
    diatance_type(const Iterator&)   // distance_type()就是一个距离类型的空指针
    {
        return 0;
    }
    // 获取值类型指针
    template <class Iterator>
    inline typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&){
        return 0;
    }


    // ------内联distance计算迭代器距离函数--------
    // 随机访问迭代器，重载计算函数，第三个输入参数仅仅用于区分重载函数
    template <class RandomAccessIterator>
    inline typename iterator_traits<RandomAccessIterator>::difference_type
    __distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
    {
        return last - first;
    }
    //普通输入迭代器，其他的访问迭代器继承该计算函数
    template <class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    __distance(InputIterator first, InputIterator last, input_iterator_tag)
    {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while(first != last){
            first++;
            n++;
        }
        return n;
    }
    // distance内联函数外部接口, 可以理解成上面两个函数的统一接口？
    template <class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    __diatance(InputIterator first, InputIterator last)
    {
        return __distance(first, last, iterator_category(first));
    }


    // ---------advance移动迭代器函数-------------
    //随机访问迭代器
    template<class RandomAccessIterator, class Distance>
    inline void __advance(RandomAccessIterator & i, Distance n, random_access_iterator_tag)
    {
        i += n;
    }
    //双向迭代器
    template<class BidirectionalIterator, class Distance>
    inline void __advance(BidirectionalIterator & i, Distance n, bidirectional_iterator_tag)
    {
        while(n){
            if(n<0){
                i--;
                n++;
            }
            if(n>0){
                i++;
                n--;
            }
        }
    }
    // 普通迭代器
    template<class InputIterator, class Distance>
    inline void __advance(InputIterator & i, Distance n, input_iterator_tag)
    {
        while(n){
            i++;
            n--;
        }
    }
    // advance函数的对外统一接口
    template<class InputIterator, class Distance>
    inline void __advance(InputIterator & i, Distance n)
    {
        __advance(i, n, iterator_category(i));
    }
}

#endif