#ifndef __UNSORTEDMAP_H__
#define __UNSORTEDMAP_H__

#include <iostream>
#include <initializer_list>
#include <iterator>
#include <cstddef>
#include <utility>
#include <vector>
#include <type_traits>
#include <limits>
#include <concepts>

/**
 * @brief General namespace
 * 
 */
namespace com {
    template<class T>
    concept Keyable = (requires(T a_, T b_) {a_ == b_;}) && (!std::is_integral<T>::value);

    template<class T>
    concept DefaultInitializableKeyable = Keyable<T> && std::default_initializable<T>;

    /**
     * @brief Container that stores pairs of key / value respecting the insert order.
     *        It can be described as a vector with map functionality.
     * 
     * @tparam key_   Type of the key.
     * @tparam value_ Type of the value.
     * @tparam delta_ Number of new elements to allocate every time the container growths.
     */
    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_ = 100>
    class vectormap
    {
        public:
            class Iterator;
            class ConstIterator;

            /** @cond */
            using key_type = key_;
            using mapped_type = value_;
            using value_type = std::pair<const key_type, mapped_type>;
            using allocator_type = std::allocator<value_type>;
            using allocator_traits = std::allocator_traits<allocator_type>;
            using reference = value_type&;
            using const_reference = const value_type&;
            using pointer = value_type*;
            using const_pointer = const value_type*;
            using iterator = Iterator;
            using const_iterator = ConstIterator;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;
            using size_type = size_t;
            using iterator_pos = std::pair<iterator, size_type>;
            
            static constexpr size_type npos = std::numeric_limits<size_type>::max();

            /** @endcond */

            class Iterator {
                public:
                    using iterator_category = std::bidirectional_iterator_tag;
                    using value_type = typename vectormap::value_type;
                    using difference_type = std::ptrdiff_t;
                    using pointer = value_type*;
                    using reference = value_type&;

                    Iterator(pointer ptr = nullptr) : ptr_(ptr) {}
                    reference operator*() const { return *ptr_; }
                    pointer operator->() const { return ptr_; }
                    iterator operator+(int other) { return iterator(ptr_ + other); }
                    iterator operator-(int other) { return iterator(ptr_ - other); }
                    iterator& operator++() { ++ptr_; return *this; }
                    iterator operator++(int) { iterator tmp = *this; ++ptr_; return tmp; }
                    iterator& operator--() { --ptr_; return *this; }
                    iterator operator--(int) { iterator tmp = *this; --ptr_; return tmp; }
                    bool operator==(const iterator& other) const { return ptr_ == other.ptr_; }
                    bool operator!=(const iterator& other) const { return ptr_ != other.ptr_; }

                    operator ConstIterator() const { return static_cast<const_pointer>(ptr_); }

                private:
                    pointer ptr_;
                    friend class ConstIterator;
            };

            class ConstIterator {
                public:
                    using iterator_category = std::bidirectional_iterator_tag;
                    using value_type = const vectormap::value_type;
                    using difference_type = std::ptrdiff_t;
                    using pointer = const value_type*;
                    using reference = const value_type&;

                    ConstIterator(pointer ptr = nullptr) : ptr_(ptr) {}
                    reference operator*() const { return *ptr_; }
                    pointer operator->() const { return ptr_; }
                    ConstIterator operator+(int other) { return const_iterator(ptr_ + other); }
                    ConstIterator operator-(int other) { return const_iterator(ptr_ - other); }
                    ConstIterator& operator++() { ++ptr_; return *this; }
                    ConstIterator operator++(int) { ConstIterator tmp = *this; ++ptr_; return tmp; }
                    ConstIterator& operator--() { --ptr_; return *this; }
                    ConstIterator operator--(int) { ConstIterator tmp = *this; --ptr_; return tmp; }
                    bool operator==(const ConstIterator& other) const { return ptr_ == other.ptr_; }
                    bool operator!=(const ConstIterator& other) const { return ptr_ != other.ptr_; }

                private:
                    pointer ptr_;
            };        
            
            
            /** @name Constructors */
            /** @{ */

            /**
             * @brief Default constructor.
             * 
             */
            vectormap() : capacity_(0), size_(0), data_(nullptr) {};

            /**
             * @brief Construct a new vectormap object from a list.
             * 
             * @param il List with the elements.
             */
            vectormap(const std::initializer_list<value_type>& il);

            /**
             * @brief Copy constructor.\n 
             *        Constructs a new vectormap object from another vectormap object.
             * 
             * @param other 
             */
            vectormap(const vectormap& other);

            /**
             * @brief Construct a new vectormap object
             * 
             * @param other 
             */
            vectormap(vectormap&& other) noexcept(allocator_traits::is_always_equal::value);
            /** @} */

            // Destructor
            ~vectormap();


            /** @name Element insertion
             */
            /** @{ */
            iterator insert(const value_type& val, const size_type pos);
            iterator insert(const key_type& key, const mapped_type& val, const size_type pos) { return insert(std::make_pair<>(key, val), pos); }
            iterator insert(const std::initializer_list<value_type>& il, const size_type pos);
            iterator insert(const vectormap<key_type, mapped_type, delta_>& map, const size_type pos);
            /**
             * @brief Adds an element at the end of the vectormap.
             * 
             * @param val        Element to be added at the end of the vectormap.
             * @return iterator  Iterator pointing to the added element.
             */
            iterator push_back(const value_type& val) { return insert(val, size_); }
            /**
             * @brief Adds an element at the end of the vectormap.
             * 
             * @param key        The key of the element.
             * @param val        The value of the element.
             * @return iterator  Iterator pointing to the added element.
             */
            iterator push_back(const key_type& key, const mapped_type& val) { return insert(key, val, size_); }
            iterator push_back(const std::initializer_list<value_type>& il) { return insert(il, size_); }
            iterator push_back(const vectormap<key_type, mapped_type, delta_>& map) { return insert(map, size_); }
            iterator push_front(const value_type& val) { return insert(val, 0); }
            iterator push_front(const key_type& key, const mapped_type& val) { return insert(key, val, 0); }
            iterator push_front(const std::initializer_list<value_type>& il) { return insert(il, 0); }
            iterator push_front(const vectormap<key_type, mapped_type, delta_>& map) { return insert(map, 0); }
            /** @} */

            /** @name Element access */
            /** @{ */
            iterator get(const size_type pos) { return pos < size_ ? iterator(&data_[pos]) : end(); }
            std::vector<iterator_pos> get(const key_type& key, size_type ordinal = 1, size_type number = 1);
            std::vector<iterator_pos> get_all(const key_type& key);      
            mapped_type& get_value(const size_type& pos) { return pos < size_ ? data_[pos].second : void_mapped_type_; }
            std::vector<mapped_type> get_value(const key_type& key, size_type ordinal = 1, size_type number = 1);
            std::vector<mapped_type> get_all_values(const key_type& key);
            const key_type& get_key(const size_type& pos) { return pos < size_ ? data_[pos].first : void_key_type_; }
            std::vector<size_type> get_pos(const key_type& key, size_type ordinal = 1, size_type number = 1);
            std::vector<size_type> get_all_pos(const key_type& key);
            pointer data() { return data_; }
            /** @} */

            /** @name  Element modification */
            /** @{ */
            void set(const value_type& new_value, const size_type pos) { data_[pos] = new_value; }
            void set(const value_type& new_value, const key_type& key, size_type ordinal = 1) { get(key, ordinal).first = new_value; }
            void set_value(const mapped_type& new_mapped_value, const size_type pos) { data_[pos].second = new_mapped_value; }
            void set_value(const mapped_type& new_mapped_value, const key_type& key, size_type ordinal = 1){ get_value(key, ordinal) = new_mapped_value; }
            void set_key(const key_type& new_key, const size_type pos) { data_[pos].first = new_key; }
            void set_key(const key_type& new_key, const key_type& key, size_type ordinal = 1) { get(key, ordinal).first->first = new_key; }
            /** @} */

            /** @name  Element management */
            /** @{ */
            void clear();
            void erase(const size_type pos);
            void erase(const key_type& key) { erase(get(key).second);  }
            void erase(const std::initializer_list<size_type>& il);
            void erase_all(const key_type& key);
            void move(const size_type from, const size_type to);
            void swap(const size_type from, const size_type to);
            void swap(vectormap& a, vectormap& b);
            /** @} */
            
            /** @name  Memory manipulation */
            /** @{ */
            size_type size() { return size_; }
            size_type capacity() { return capacity_; }
            bool is_empty() { return ((data_ == nullptr) || (size_ == 0)); }
            bool reserve(size_type min_capacity);
            bool shrink() { return resize(size_); };
            bool resize(size_type new_capacity);
            /** @} */

            /** @name  Operators */
            /** @{ */
            vectormap& operator=(const vectormap& other);
            vectormap& operator=(vectormap&& other);
            /** @} */

            /** @name  Iterators */
            /** @{ */
            iterator begin() { return iterator(data_); }
            iterator end() { return iterator(data_ + size_); }

            reverse_iterator rbegin() { return reverse_iterator(data_ + size_); }
            reverse_iterator rend() { return reverse_iterator(data_); }

            const_iterator begin() const { return const_iterator(data_); }
            const_iterator end() const { return const_iterator(data_ + size_); }
            const_iterator cbegin() const { return const_iterator(data_); }
            const_iterator cend() const { return const_iterator(data_ + size_); }

            const_reverse_iterator rbegin() const { return const_reverse_iterator(data_); }
            const_reverse_iterator rend() const { return const_reverse_iterator(data_ + size_); }
            const_reverse_iterator crbegin() const { return const_reverse_iterator(data_ + size_); }
            const_reverse_iterator crbend() const { return const_reverse_iterator(data_); }

            iterator last() { return iterator(data_ + size_ - 1); }
            const_iterator last() const { return const_iterator(data_ + size_ - 1); }
            /** @} */

            

        private:
            allocator_type allocator_;
            size_type size_ = 0;
            size_type capacity_ = 0;
            pointer data_ = nullptr;
            mapped_type void_mapped_type_;
            key_type void_key_type_;
            bool gap_(size_type from, size_type length);
    };

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    vectormap<key_, value_, delta_>::vectormap(const std::initializer_list<value_type>& il) : capacity_(delta_), size_(0) {
        if (capacity_ < il.size()) {
            capacity_ = ((il.size() / delta_) + 1) * delta_;
        }

        data_ = allocator_traits::allocate(allocator_, capacity_);

        size_type counter = 0;
        for (auto elem : il) {
            allocator_traits::construct(allocator_, data_ + counter, elem);
            size_++;
            counter++;
        }
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    vectormap<key_, value_, delta_>::vectormap(const vectormap &other) : capacity_(other.capacity_), size_(other.size_) {
        data_ = allocator_traits::allocate(allocator_, capacity_);

        for (size_type i = 0; i < size_; ++i) {
            allocator_traits::construct(allocator_, data_ + i, std::make_pair(other.data_[i].first, other.data_[i].second));
        }
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    vectormap<key_, value_, delta_>::vectormap(vectormap &&other) noexcept(allocator_traits::is_always_equal::value) {
        if (allocator_traits::propagate_on_container_move_assignment::value) {
            allocator_ = std::move(other.allocator_);
        }
        else {
            allocator_ = other.allocator_;
        }

        data_ = std::exchange(other.data_, nullptr);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    vectormap<key_, value_, delta_>::~vectormap() {
        for (size_type i = 0; i < size_; i++) {
            allocator_traits::destroy(allocator_, data_ + i);
        }

        allocator_traits::deallocate(allocator_, data_, capacity_);
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_> 
    vectormap<key_, value_, delta_>::iterator vectormap<key_, value_, delta_>::insert(const value_type &val, const size_type pos) {
        if (pos > size_) {
            return end();
        }
        else {
            bool success = gap_(pos, 1);
            
            if (success) {            
                allocator_traits::construct(allocator_, data_ + pos, val);
                return iterator(&data_[pos]);
            }
            else {
                return end();
            }
        }
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    vectormap<key_, value_, delta_>::iterator vectormap<key_, value_, delta_>::insert(const std::initializer_list<value_type>& il, const size_type pos) {
        if (pos > size_) {
            return end();
        }
        else {
            bool success = gap_(pos, il.size());
            if (success) {        
                for (size_type i = 0; i < il.size(); i++) {
                    allocator_traits::construct(allocator_, data_ + pos + i, *(il.begin() + i));
                }
                return iterator(data_ + pos);
            }
            else {
                return end();
            }
        }
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    vectormap<key_, value_, delta_>::iterator vectormap<key_, value_, delta_>::insert(const vectormap<key_type, mapped_type, delta_>& map, const size_type pos) {
        if (pos > size_) {
            return end();
        }
        else {
            bool success = gap_(pos, map.size_);
            if (success) {                
                for (size_type i = 0; i < map.size_; i++) {
                    allocator_traits::construct(allocator_, data_ + pos + i, 
                        std::make_pair<>(map.data_[i].first, map.data_[i].second));
                }
                return iterator(data_ + pos);
            }
            else {
                return end();
            }
        }
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    std::vector<typename vectormap<key_, value_, delta_>::iterator_pos> vectormap<key_, value_, delta_>::get(const key_type& key, const size_type ordinal, size_type number) {
        std::vector<iterator_pos> out;
        size_type order = 1;
        
        for (size_type i = 0; i < size_; ++i) {
            if (data_[i].first == key) {
                if ((order >= ordinal) && (out.size() == (number - 1))) {
                    out.push_back(std::make_pair(iterator(&data_[i]), i));
                    return std::move(out);
                }
                else if (order >= ordinal) {
                    out.push_back(std::make_pair(iterator(&data_[i]), i));
                    ++order;
                }
                else {
                    ++order;
                }
            }
        }

        return std::move(out);
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    std::vector<typename vectormap<key_, value_, delta_>::iterator_pos> vectormap<key_, value_, delta_>::get_all(const key_type& key) {
        std::vector<iterator_pos> out;
        for (size_type i = 0; i < size_; i++)
            if (data_[i].first == key) {
                out.push_back(std::make_pair<>(&data_[i], i));
            }

        return out;
    }

    template <DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    inline std::vector<typename vectormap<key_, value_, delta_>::mapped_type> vectormap<key_, value_, delta_>::get_value(const key_type &key, size_type ordinal, size_type number)
    {
        std::vector<mapped_type> out;
        std::vector<iterator_pos> get_ = get(key, ordinal, number);

        for (size_type i = 0; i < get_.size(); ++i) {
            out.push_back(get_.at(i).first->second);
        }

        return std::move(out);
    }

    template <DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    std::vector<typename vectormap<key_, value_, delta_>::mapped_type> vectormap<key_, value_, delta_>::get_all_values(const key_type &key)
    {
        std::vector<mapped_type> out;
        for (auto elem : get_all(key)) {
            out.push_back(elem.first->second);
        }

        return out;
    }

    template <DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    inline std::vector<typename vectormap<key_, value_, delta_>::size_type> vectormap<key_, value_, delta_>::get_pos(const key_type &key, size_type ordinal, size_type number)
    {
        std::vector<mapped_type> out;
        std::vector<iterator_pos> get_ = get(key, ordinal, number);

        for (size_type i = 0; i < get_.size(); ++i) {
            out.push_back(get_.at(i).second);
        }

        return std::move(out);
    }

    template <DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    inline std::vector<typename vectormap<key_, value_, delta_>::size_type> vectormap<key_, value_, delta_>::get_all_pos(const key_type &key)
    {
        std::vector<size_type> out;
        for (auto elem : get_all(key)) {
            out.push_back(elem.second);
        }

        return out;
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    void vectormap<key_, value_, delta_>::clear() {
        for (size_type i = 0; i < size_; i++)
            allocator_traits::destroy(allocator_, data_ + i);
        size_ = 0;
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    void vectormap<key_, value_, delta_>::erase(const size_type pos) {
        if ((size_ > 0) && (pos < size_)) {
            --size_;
            for (size_type i = pos; i < size_; ++i) {
                allocator_traits::destroy(allocator_, data_ + i);
                allocator_traits::construct(allocator_, data_ + i, std::move(data_[i + 1]));
            }
        }
    }

    template <DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    inline void vectormap<key_, value_, delta_>::erase(const std::initializer_list<size_type> &il) {
        for (auto elem : il) {
            erase(elem);
        }
    }

    template <DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    void vectormap<key_, value_, delta_>::erase_all(const key_type &key)
    {
        auto v = get_all(key);
        for (auto it = v.rbegin(); it != v.rend(); ++it) {
            erase(it->second);
        }
    }

    template <DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    inline void vectormap<key_, value_, delta_>::move(const size_type from, const size_type to) {
        if ((from < size_) && (to < size_)) {
            gap_(to, 1);
            allocator_traits::construct(allocator_, data_ + to, std::move(data_[from + 1]));
            allocator_traits::destroy(allocator_, data_ + from + 1);
            for (size_type i = from + 1; i < size_; ++i) {
                allocator_traits::construct(allocator_, data_ + i, std::move(data_[i + 1]));
                allocator_traits::destroy(allocator_, data_ + i + 1);
            }
            --size_;
        }        
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    inline void vectormap<key_, value_, delta_>::swap(const size_type from, const size_type to)
    {
        if ((from < size_) && (to < size_)) {
            value_type temp_ = data_[to];

            allocator_traits::destroy(allocator_, data_ + to);
            allocator_traits::construct(allocator_, data_ + to, std::move(data_[from]));
            allocator_traits::destroy(allocator_, data_ + from);
            allocator_traits::construct(allocator_, data_ + from, temp_);
        }
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_> 
    inline void vectormap<key_, value_, delta_>::swap(vectormap &a, vectormap &b) {
        std::swap(a.allocator_, b.allocator_);
        std::swap(a.data_, b.data_);
        std::swap(a.size_, b.size_);
        std::swap(a.capacity_, b.capacity_);
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    inline bool vectormap<key_, value_, delta_>::reserve(size_type min_capacity) {
        if (min_capacity < size_)
            return false;

        size_type new_capacity = ((min_capacity / delta_) + 1) * delta_;
        return resize(new_capacity);
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    bool vectormap<key_, value_, delta_>::resize(size_type new_capacity) {
        if (new_capacity < size_)
            return false;

        pointer new_data = allocator_traits::allocate(allocator_, new_capacity);
        for (size_type i = 0; i < size_ ; i++) {
            allocator_traits::construct(allocator_, new_data + i, data_[i]);
            allocator_traits::destroy(allocator_, data_ + i);
        }
        
        if (size_ != 0) {
            allocator_traits::deallocate(allocator_, data_, capacity_);
        }

        data_ = new_data;
        capacity_ = new_capacity;

        return true;
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    vectormap<key_, value_, delta_> &vectormap<key_, value_, delta_>::operator=(const vectormap& other) {
        if (this != &other) {
            clear();
            if (other.size_ > capacity_) {
                reserve(other.size_);
            }
            
            for (size_type i = 0; i < other.size_; ++i) {
                allocator_traits::construct(allocator_, data_ + i, std::make_pair(other.data_[i].first, other.data_[i].second));
            }
        }

        return *this;
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    vectormap<key_, value_, delta_>& vectormap<key_, value_, delta_>::operator=(vectormap&& other) {
        std::swap(other.allocator_, allocator_);
        data_ = std::exchange(other.data_, nullptr);
        size_ = std::exchange(other.size_, 0);
        std::swap(other.capacity_, capacity_);

        return *this;
    }

    template<DefaultInitializableKeyable key_, std::default_initializable value_, size_t delta_>
    bool vectormap<key_, value_, delta_>::gap_(size_type from, size_type length)
    {
        bool success = true;

        if ((size_ + length) > capacity_) {
            success = reserve(size_ + length);
        }

        size_ = size_ + length;
        if (size_ <= length) {
            return true;
        }

        if (success) {
            if (from >= size_){
                return true;
            }
            else {
                for (size_type i = size_ - 1; ((i > from) && ((i - length) != npos)); i--) {
                    allocator_traits::construct(allocator_, data_ + i, std::move(data_[i - length]));
                    allocator_traits::destroy(allocator_, data_ + i - length);
                }
                return true;
            }
        }
        else {
            return false;
        }
    }
}
#endif
