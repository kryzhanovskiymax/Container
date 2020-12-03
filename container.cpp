#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };
    
    template <typename ValueType>
    class BasicIterator {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node):node_(node) {}

    public:
    
        using iterator_category = std::forward_iterator_tag;

        using value_type = Type;

        using difference_type = std::ptrdiff_t;

        using pointer = ValueType*;

        using reference = ValueType&;

        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept
            : node_(other.node_) {}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка, либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        // Оператор, проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto this_copy(*this);
            ++(*this);
            return this_copy;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка.
        // Вызов этого оператора, у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };

public:

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;
    
    template <typename IteratorType>
    SingleLinkedList(IteratorType begin, IteratorType end) : size_(std::distance(begin, end)) {
    try {
        size_ = std::distance(begin, end);
        SingleLinkedList<Type> tmp;
        for(auto iter = begin; iter != end; ++iter) {
            tmp.PushFront(*iter);
        }
        
        SingleLinkedList<Type> tmp2;
        for(auto iter = tmp.begin(); iter != tmp.end(); ++iter) {
            tmp2.PushFront(*iter);
        }
        
        this->swap(tmp2);
    }
    catch (std::bad_alloc&) {
        Clear();
        throw;
    }
}
SingleLinkedList(const SingleLinkedList& other) {
    Assign(other.begin(), other.end());
}

SingleLinkedList(std::initializer_list<Type> values) {
    Assign(values.begin(), values.end());
}
    
    SingleLinkedList() {
        head_.next_node = nullptr;
        size_ = 0;
    }
    
    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if(this != &rhs) {
            if(rhs.IsEmpty()) {
                Clear();
            }else {
                auto rhs_copy(rhs);
                swap(rhs_copy);
            }    
        }
        return *this;
    }
    
    ~SingleLinkedList() {
        Clear();
    }
    
    void PopFront() noexcept{
        auto first_element = head_.next_node;
        head_.next_node = first_element->next_node;
        --size_;
        delete first_element;
    }
    
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }
    
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        pos.node_->next_node = new Node(value, pos.node_->next_node);
        ++size_;
        return Iterator{pos.node_->next_node};
    }
    
    Iterator EraseAfter(ConstIterator pos) noexcept{
        if(pos.node_->next_node == nullptr) {
            return {};
        }
        
        auto node_to_delete = pos.node_->next_node;
        pos.node_->next_node = node_to_delete->next_node;
        --size_;
        delete node_to_delete;
        return Iterator{pos.node_->next_node};
    }
    
    void Clear() {
        while(head_.next_node != nullptr) {
            auto next_element = head_.next_node;
            head_.next_node = next_element->next_node;
            delete next_element;
        }
        
        size_ = 0;
    }
    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept {
        // Заглушка. Реализуйте метод самостоятельно
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept {
        return GetSize() == 0;
    }
    
    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }
    [[nodiscard]] Iterator end() noexcept {
        return {};
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }
    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }
    [[nodiscard]] ConstIterator cend() const noexcept {
        return {};
    }
    
    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{&head_};
    }
    
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{const_cast<SingleLinkedList<int>::Node*>(&head_)};
    }
    
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return cbefore_begin();
    }
    
    void swap(SingleLinkedList& other) {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    template <typename InputIterator>
    void Assign(InputIterator from, InputIterator to) {
        SingleLinkedList<Type> tmp;
        Node** node_ptr = &tmp.head_.next_node;
        while(from != to) {
            assert(*node_ptr == nullptr);
            *node_ptr = new Node(*from, nullptr);
            ++tmp.size_;
            node_ptr = &((*node_ptr)->next_node);
            ++from;
        }
        
        swap(tmp);
    }
    
    Node head_;
    size_t size_;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept{
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (&lhs == &rhs) || 
        ((lhs.GetSize() == rhs.GetSize()) && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (rhs < lhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}
