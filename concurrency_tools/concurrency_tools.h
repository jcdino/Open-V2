#pragma once
#include <stdint.h>
#include <atomic>

class concurrent_string {
private:
	static constexpr uint32_t internal_concurrent_string_size = 16;

	union {
		char local_data[internal_concurrent_string_size];
		struct {
			char* data;
			uint32_t length;
		} remote_data;
	} _data;
public:
	concurrent_string();
	concurrent_string(const char* source);
	concurrent_string(const char* start, const char* end);
	concurrent_string(const char* start, uint32_t size);
	concurrent_string(const concurrent_string&);
	concurrent_string(concurrent_string&&) noexcept;
	~concurrent_string();

	concurrent_string& operator=(const concurrent_string&);
	concurrent_string& operator=(concurrent_string&&) noexcept;
	concurrent_string& operator+=(const concurrent_string&);
	concurrent_string& operator+=(const char*);
	bool operator==(const char*) const;
	bool operator==(const concurrent_string&) const;
	uint32_t length() const;
	const char* c_str() const;
	void clear();
};

template <typename T>
struct concurrent_allocator {
	using value_type = T;
	concurrent_allocator() noexcept {};
	template <typename U>
	concurrent_allocator(const concurrent_allocator<U>&) noexcept {};
	T* allocate(size_t n);
	void deallocate(T* p, size_t n);
};

template <typename T, typename U>
constexpr bool operator== (const concurrent_allocator<T>&, const concurrent_allocator<U>&) noexcept {
	return true;
}

template <class T, class U>
constexpr bool operator!= (const concurrent_allocator<T>&, const concurrent_allocator<U>&) noexcept {
	return false;
}

template<typename T, uint32_t block, uint32_t index_sz>
class fixed_sz_deque_iterator;

template<typename T, uint32_t block, uint32_t index_sz>
class fixed_sz_deque {
public:
	struct entry_node {
	private:
		T e;
		std::atomic<uint32_t> next_free;
	public:
		friend class fixed_sz_deque;
		template<typename V>
		void visit(const V& v) {
			if (next_free.load(std::memory_order::memory_order_acquire) == 0)
				v(e);
		}
		template<typename V>
		void visit(const V& v) const {
			if (next_free.load(std::memory_order::memory_order_acquire) == 0)
				v(e);
		}
	};
private:
	std::atomic<entry_node*> index_array[index_sz] = { nullptr };
	std::atomic<uint32_t> first_free = 0;
	std::atomic<uint32_t> first_free_index = 1;
public:
	fixed_sz_deque();
	~fixed_sz_deque();

	T& at(uint32_t index) const;
	entry_node& node_at(uint32_t index) const;
	void free(uint32_t index);
	uint32_t past_end() const;

	template<typename ...P>
	uint32_t emplace_back(P&& ... params);

	fixed_sz_deque_iterator<T, block, index_sz> begin() const;
	fixed_sz_deque_iterator<T, block, index_sz> end() const;
};

template<typename T, uint32_t block, uint32_t index_sz>
class fixed_sz_deque_iterator {
private:
	const fixed_sz_deque<T, block, index_sz>* parent;
	int32_t position;
public:
	fixed_sz_deque_iterator() : parent(nullptr), position(0) {};
	fixed_sz_deque_iterator(const fixed_sz_deque<T, block, index_sz>& p) : parent(&p), position(0) {};
	fixed_sz_deque_iterator(const fixed_sz_deque<T, block, index_sz>& p, uint32_t o) : parent(&p), position(o) {};

	bool operator==(const fixed_sz_deque_iterator& o) const {
		return position == o.position;
	}
	bool operator!=(const fixed_sz_deque_iterator& o) const {
		return position != o.position;
	}
	typename fixed_sz_deque<T, block, index_sz>::entry_node& operator*() const {
		return parent->node_at(position);
	}
	typename fixed_sz_deque<T, block, index_sz>::entry_node& operator[](int32_t offset) const {
		return parent->node_at(position + offset);
	}
	typename fixed_sz_deque<T, block, index_sz>::entry_node* operator->() const {
		return &(parent->node_at(position));
	}
	fixed_sz_deque_iterator& operator++() {
		++position;
		return *this;
	}
	fixed_sz_deque_iterator& operator--() {
		++position;
		return *this;
	}
	fixed_sz_deque_iterator operator++(int) {
		++position;
		return fixed_sz_deque_iterator(parent, position - 1);
	}
	fixed_sz_deque_iterator operator--(int) {
		++position;
		return fixed_sz_deque_iterator(parent, position + 1);
	}
	fixed_sz_deque_iterator& operator+=(int32_t v) {
		position += v;
		return *this;
	}
	fixed_sz_deque_iterator& operator-=(int32_t v) {
		position -= v;
		return *this;
	}
	fixed_sz_deque_iterator operator+(int32_t v) {
		return fixed_sz_deque_iterator(parent, position + v);
	}
	fixed_sz_deque_iterator operator-(int32_t v) {
		return fixed_sz_deque_iterator(parent, position - v);
	}
	int32_t operator-(const fixed_sz_deque_iterator& o) {
		return position - o.position;
	}
	bool operator<(const fixed_sz_deque_iterator& o) const {
		return position < o.position;
	}
	bool operator<=(const fixed_sz_deque_iterator& o) const {
		return position <= o.position;
	}
	bool operator>(const fixed_sz_deque_iterator& o) const {
		return position > o.position;
	}
	bool operator>=(const fixed_sz_deque_iterator& o) const {
		return position >= o.position;
	}
};

template<typename T, uint32_t block, uint32_t index_sz>
class std::iterator_traits<fixed_sz_deque_iterator<T, block, index_sz>> {
public:
	using difference_type = int32_t;
	using value_type = typename fixed_sz_deque<T, block, index_sz>::entry_node;
	using pointer = typename fixed_sz_deque<T, block, index_sz>::entry_node*;
	using reference = typename fixed_sz_deque<T, block, index_sz>::entry_node&;
	using iterator_category = std::random_access_iterator_tag;
};