#ifndef __XY_CIRCULAR_ARRAY_QUEUE_HPP_47831C93_EDAA_44B9_94F2_6FF36839D1A6__ 
#define __XY_CIRCULAR_ARRAY_QUEUE_HPP_47831C93_EDAA_44B9_94F2_6FF36839D1A6__

template <typename ElementType>
class XYCircularArrayQueue
{
public:
    typedef ElementType                 value_type;
    typedef ElementType&                reference;
    typedef const ElementType&          const_reference;
public:
    /***
     *  @brief  Default constructor creates no elements.
     **/
    explicit XYCircularArrayQueue()
        :_elements(NULL),
        _head(0),
        _tail(0),
        _CAPACITY(0)
    {
    }

    constexpr ~XYCircularArrayQueue()
    noexcept
    {
        delete[]  _elements;
    }

    [[nodiscard]] inline int init(const std::size_t  capacity)
    {
        _CAPACITY = capacity;
        _elements = new value_type[_CAPACITY];
        return _elements ? 0 : -1;
    }

    [[nodiscard]] constexpr inline std::size_t capacity()
    noexcept
    {
        return _CAPACITY;
    }

    [[nodiscard]] constexpr inline bool full() const
    noexcept
    {
        return size() >= (_CAPACITY -1);
    }

    [[nodiscard]] constexpr inline bool empty() const
    noexcept
    {
        return _head == _tail;
    }

    /***
     * @return: the number of elements in the @queue.
     **/
    [[nodiscard]] constexpr inline std::size_t size() const
    noexcept
    {
        return (_CAPACITY -_head+_tail) %_CAPACITY;
    }

    /***
     * @return: the number of free elements in the @queue. 
     **/
    [[nodiscard]] constexpr inline std::size_t free() const
    noexcept
    {
        return _CAPACITY - (_CAPACITY -_head+_tail) %_CAPACITY;
    }

    /***
     *  @return: a read/write reference to the i-th element.
     **/
    [[nodiscard]] constexpr inline reference get_at(int i)
    noexcept
    {
        i += _head;
        i %= _CAPACITY;
        return _elements[i];
    }

    /***
     *  @return: a read-only reference to the i-th element.
     **/
    [[nodiscard]] constexpr inline const_reference get_at(int i) const
    noexcept
    {
        i += _head;
        i %= _CAPACITY;
        return _elements[i];
    }

    /***
     *  @return: a read/write reference to the last element
     **/
    [[nodiscard]] constexpr inline reference back()
    noexcept
    {
        return _elements[(_CAPACITY +_tail -1) % _CAPACITY];
    }

    /***
     *  @return: a read-only reference to the last element
     **/
    [[nodiscard]] constexpr inline const_reference back() const
    noexcept
    {
        return _elements[(_CAPACITY +_tail -1) % _CAPACITY];
    }

    /***
     *  @return: 0 if succeeded, -1 if failed
     **/
    [[nodiscard]] constexpr inline int push_back(const const_reference value)
    noexcept
    {
        if(size() == (_CAPACITY -1))
        {
            return -1;
        }
        else
        {
            _elements[_tail] = value;
            _tail = (_tail+1) % _CAPACITY;
            return 0;
        }
    }

    [[nodiscard]] constexpr inline reference inc_1_at_tail()
    noexcept
    {
        _tail++;
        _tail += _CAPACITY;
        _tail %= _CAPACITY;
        return _elements[(_CAPACITY +_tail -1) % _CAPACITY];
    }

    constexpr inline void pop_front()
    noexcept
    {
        if(empty())
        {
            return;
        }
        else
        {
            _head = (_head + 1) % _CAPACITY;
        }
    }

    constexpr inline void pop_back()
    noexcept
    {
        if(empty())
        {
            return;
        }
        else
        {
            _tail = (_tail - 1 + _CAPACITY) % _CAPACITY;
        }
    }

    constexpr inline void pop_last_n(const int n)
    noexcept
    {
        if (size()<n)
        {
            _head = _tail = 0;
        }
        else
        {
            _tail -= n;
            _tail += _CAPACITY;
            _tail %= _CAPACITY;
        }
    }
private:
    value_type* _elements;
    std::int32_t  _head;
    std::int32_t  _tail;
    std::size_t   _CAPACITY;

};

#endif /* #ifndef __XY_CIRCULAR_ARRAY_QUEUE_HPP_47831C93_EDAA_44B9_94F2_6FF36839D1A6__ */

