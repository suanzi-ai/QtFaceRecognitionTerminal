#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <climits>
#include <cstddef>

template <typename T, size_t BlockSize = 4096>
class MemoryPool {
 public:
  // 使用 typedef 简化类型书写
  typedef T* pointer;

  // 定义 rebind<U>::other 接口
  template <typename U>
  struct rebind {
    typedef MemoryPool<U> other;
  };

  // 默认构造
  // C++11 使用了 noexcept 来显式的声明此函数不会抛出异常
  MemoryPool() noexcept {
    currentBlock_ = nullptr;
    currentSlot_ = nullptr;
    lastSlot_ = nullptr;
    freeSlots_ = nullptr;
  }

  // 销毁一个现有的内存池
  ~MemoryPool() noexcept {
    // 循环销毁内存池中分配的内存区块
    slot_pointer_ curr = currentBlock_;
    while (curr != nullptr) {
      slot_pointer_ prev = curr->next;
      operator delete(reinterpret_cast<void*>(curr));
      curr = prev;
    }
  }

  // 同一时间只能分配一个对象, n 和 hint 会被忽略
  pointer allocate(size_t n = 1, const T* hint = 0) {
    if (freeSlots_ != nullptr) {
      pointer result = reinterpret_cast<pointer>(freeSlots_);
      freeSlots_ = freeSlots_->next;
      return result;
    } else {
      if (currentSlot_ >= lastSlot_) {
        // 分配一个内存区块
        data_pointer_ newBlock =
            reinterpret_cast<data_pointer_>(operator new(BlockSize));
        reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
        currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);
        data_pointer_ body = newBlock + sizeof(slot_pointer_);
        uintptr_t result = reinterpret_cast<uintptr_t>(body);
        size_t bodyPadding =
            (alignof(slot_type_) - result) % alignof(slot_type_);
        currentSlot_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
        lastSlot_ = reinterpret_cast<slot_pointer_>(newBlock + BlockSize -
                                                    sizeof(slot_type_) + 1);
      }
      return reinterpret_cast<pointer>(currentSlot_++);
    }
  }

  // 销毁指针 p 指向的内存区块
  void deallocate(pointer p, size_t n = 1) {
    if (p != nullptr) {
      reinterpret_cast<slot_pointer_>(p)->next = freeSlots_;
      freeSlots_ = reinterpret_cast<slot_pointer_>(p);
    }
  }

  // 调用构造函数, 使用 std::forward 转发变参模板
  template <typename U, typename... Args>
  void construct(U* p, Args&&... args) {
    new (p) U(std::forward<Args>(args)...);
  }

  // 销毁内存池中的对象, 即调用对象的析构函数
  template <typename U>
  void destroy(U* p) {
    p->~U();
  }

 private:
  // 用于存储内存池中的对象槽
  union Slot_ {
    T element;
    Slot_* next;
  };

  // 数据指针
  typedef char* data_pointer_;
  // 对象槽
  typedef Slot_ slot_type_;
  // 对象槽指针
  typedef Slot_* slot_pointer_;

  // 指向当前内存区块
  slot_pointer_ currentBlock_;
  // 指向当前内存区块的一个对象槽
  slot_pointer_ currentSlot_;
  // 指向当前内存区块的最后一个对象槽
  slot_pointer_ lastSlot_;
  // 指向当前内存区块中的空闲对象槽
  slot_pointer_ freeSlots_;
  // 检查定义的内存池大小是否过小
  // static_assert(BlockSize >= 2 * sizeof(slot_type_), "BlockSize too small.");
};

#endif  // MEMORY_POOL_HPP
