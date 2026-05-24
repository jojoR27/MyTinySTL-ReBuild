// 该文件是MyTinySTL的编译期类型判断工具
// STL容器会根据这个标签：POD类型 = 纯数据类型 → 可以直接拷贝内存，无需执行构造 / 析构，效率极高。  | 非 POD 类型 → 调用构造 / 析构函数，保证安全

#ifndef MYTINYSTL_TYPE_TRAITS_H_
#define MYTINYSTL_TYPE_TRAITS_H_

// 自定义命名空间---标记某个类型的某个属性是否成立
namespace mytiny
{
    struct _true_type {};
    struct _false_type {};


    // 通用模板类---数据类型类，默认该数据类型不是pod类型
    template <class T>
    struct type_traits
    {
        typedef _false_type has_trivial_default_ctor;  // 是否有平凡(trivial)默认构造函数
        typedef _false_type has_trivial_copy_ctor;  // 平凡拷贝构造函数
        typedef _false_type has_trivial_assignment_operator;  // 平凡赋值运算符
        typedef _false_type has_trivial_dtor;  // 平凡析构函数
        typedef _false_type is_POD_type;  //默认不是POD类型
    };


    // 全特化通用模板类---给下面具体的基础数据类型定义为POD类型
    template <> 
    struct type_traits<char>{
        typedef _true_type is_POD_type;
    };

    template <> 
    struct type_traits<unsigned char>{
        typedef _true_type is_POD_type;
    };

    template <> 
    struct type_traits<signed char>{
        typedef _true_type is_POD_type;
    };

    template <> 
    struct type_traits<short>{
        typedef _true_type is_POD_type;
    };

    template <> 
    struct type_traits<unsigned short>{
        typedef _true_type is_POD_type;
    };

    template <> 
    struct type_traits<int>{
        typedef _true_type is_POD_type;
    };

    template <> 
    struct type_traits<unsigned int>{
        typedef _true_type is_POD_type;
    };

    template <> 
    struct type_traits<long>{
        typedef _true_type is_POD_type;
    };

    template <> 
    struct type_traits<unsigned long>{
        typedef _true_type is_POD_type;
    };

    template <> 
    struct type_traits<long long>{
        typedef _true_type is_POD_type;
    };

    template <> 
    struct type_traits<unsigned long long>{
        typedef _true_type is_POD_type;
    };

    template <> 
    struct type_traits<float>{
        typedef _true_type is_POD_type;
    };

    template <> 
    struct type_traits<double>{
        typedef _true_type is_POD_type;
    };

    template <> 
    struct type_traits<long double>{
        typedef _true_type is_POD_type;
    };


    // 通用模板类偏特化---为任意类型的指针这一类数据类型定义为POD类型
    template<class T>
    struct type_traits<T*>{
        typedef _true_type is_POD_type;
    };
}

#endif