// vector容器

#ifndef _MYTINYSTL_VECTOR_H_
#define _MYTINYSTL_VECTOR_H_

#include <cassert>
#include <utility>
#include "uninitialized.h"
#include "allocator.h"

namespace mytiny
{
    // 容器内元素类型 T, Alloc分配内存的类
    template<class T, class Alloc = allocator<T>>
    class vector
    {
    public:
        using iterator = T*;
        using const_iterator = const T*;
        using size_type = size_t;
        using value_type = T;

        // vector默认构造
        vector() : _start(nullptr), _finish(nullptr), _end_of_storage(nullptr){}
        // eg:vector<int>v(10,9)
        explicit vector(size_type n, const T& val = T())
        {
            if(n==0)
            {
                _start = _finish = _end_of_storage = nullptr;
                return;
            }
            _start = allocate_space(n);
            _finish = uninitialized_fill_n(_start, n, val);
            _end_of_storage = _start + n;
        }
        // eg:vector<int>v(a.begin(),a.end())
        template<class InputIter>
        vector(InputIter first, InputIter last)
        {
            size_type len = last - first;
            _start = allocate_space(len);
            _finish = uninitialized_copy(first, last, _start);
            _end_of_storage = _start + len;
        }
        // 拷贝构造
        vector(const vector& rhs)
        {
            size_type cap = rhs.capacity()；
            _start = allocate_space(cap);
            _finish = uninitialized_copy(rhs.begin(), rhs.end(), _start);
            _end_of_storage = _start + cap;
        }
        // 赋值运算符重载
        vector & operator=(const vector& rhs)
        {
            if(this == &rhs)
                return *this;
            destroy(_start, _finish);
            dellocate_space(_start, this.capacity());

            size_type cap = rhs.capacity();
            _start = allocate_space(cap);
            _finish = uninitialized_copy(rhs.begin(), rhs.end(), _start);
            _end_of_storage = _start + cap;
            return *this;
        }
        // 析构
        ~vector()
        {
            destory(_start, _finish);
            deallocate_space(_start, this.capacity());
            _start = _finish = _end_of_storage = nullptr;
        }

        // 各种接口
        iterator begin() noexcept {return _start;}
        iterator end() noexcept{return _finish;}
        const_iterator begin() const noexcept {return _start;}
        const_iterator end() const noexcept{return _finish;}
        size_type size() const noexcept{return size_type(_finish - _start);}
        size_type capacity() const noexcept{return size_type(_end_of_storage - _start);}
        bool empty() const noexcept{ return _start = _finish;}
        // 重分配内存区 搬运
        void reserve(size_type n)
        {
            if(n<=capacity())return;
            iterator new_start = allocate_space(n);
            size_type sz = this.size();
            uninitialized_copy(_start, _finish, new_start);

            destory(_start, _finish);
            deallocate_space(_start, this.capacity());
            _start = new_start;
            _finish = new_start + sz;
            _end_of_storage = new_start + n;
        }
        void resize(size_type n, const T& val = T())
        {
            if(n<size()){
                destory(_start + n, _finish);
                _finish = _start + n;
            }
            else if(n>size()){
                reserve(n);
                // 尾部填充val
                _finish = uninitialized_fill_n(_finish, n-size(),val);
            }
        }
        // 方括号运算符重载
        T& operator[](size_type pos){
            assert(pos < size()); // 安全检查，cpp内置的
            return _start[pos];
        }
        const T& operator[](size_type pos) const
        {
        assert(pos < size());
        return _start[pos];
        }
        T& front(){return *begin();}
        T& back(){return *(end()-1);}

        // 增删改查操作
        void push_back(const &T val){
            if(_finish == _end_of_storage)
            {
                reserve(capacity()==0?1:capacity()*2);
            }
            construct(_finish,val);
            ++finish;
        }
        void pop_back(){
            assert(!this->empty());
            --finish;
            destory(_finish);
        }
        // 不释放内存
        void clear(){
            destory(_start, _finish);
            _finish = _start;
        }
        void swap(vector & rhs)noexcept
        {
            std::swap(_start, rhs._start);
            std::swap(_finish, rhs._finish);
            std::swap(_end_of_storage, rhs._end_of_storage);
        }

    private:
        iterator _start;
        iterator _finish;
        iterator _end_of_storage;
        Alloc _alloc;

        iterator allocate_space(size_type n)
        {
            _alloc.deallocate(p, n);
        }
    };
}

#endif