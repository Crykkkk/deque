#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {
  
template<class T>
class double_list {
public:
	struct node{
		T data;
		node* prev = nullptr;
		node* next = nullptr;
		node (T d, node* p, node* n) : data(d), prev(p), next(n) {}
		node (){}
	};

	int size = 0;
	node* head_ = new node();
	node* tail_ = new node();
	// --------------------------
	/**
	 * the follows are constructors and destructors
	 * you can also add some if needed.
	 */
	double_list() {
		head_->next = tail_;
		tail_->prev = head_;
	}
	double_list(const double_list<T> &other) {
		head_->next = tail_;
		tail_->prev = head_;
		size = 0;
		node* track = other.head_->next;
		while (track != other.tail_) {
			insert_tail(track->data);
			track = track->next;
		}
	}

	double_list &operator=(const double_list<T> &other){
		if (this == &other) return *this;
		clear();
		node* track = other.head_->next;
		while (track != other.tail_) {
			insert_tail(track->data);
			track = track->next;
		}		
		return *this;
	}

	void clear (){
		node* curr = head_->next;
		while (curr != tail_) {
			node* next_node = curr->next;
			delete curr;
			curr = next_node;
		}
		head_->next = tail_;
		tail_->prev = head_;
		size = 0;
	}
	~double_list() {
		clear();
		delete head_;
		delete tail_;
	}

	class iterator {
	public:
		/**
		 * elements
		 * add whatever you want
		 */
		 node* at = nullptr;
		// --------------------------
		/**
		 * the follows are constructors and destructors
		 * you can also add some if needed.
		 */
		iterator() {}
		iterator(const iterator &t) {
			at = t.at;
		}
		iterator(node* a):at(a){}
		~iterator() {} // 应该不能 delete 吧？
		
		/**
		 * iter++
		 */
		iterator operator++(int) {
			if (at == nullptr || at->next == nullptr) throw sjtu::invalid_iterator();
			iterator tmp = *this;
			at = at->next;
			return tmp;	
		}
		/**
		 * ++iter
		 */
		iterator &operator++() {
			if (at == nullptr|| at->next == nullptr) throw sjtu::invalid_iterator();
			at = at->next;
			return *this; // 引用的返回引用
		}
		/**
		 * iter--
		 */
		iterator operator--(int) {
			if (at == nullptr|| at->prev == nullptr|| at->prev->prev == nullptr) throw sjtu::invalid_iterator();
			iterator tmp = *this;
			at = at->prev;
			return tmp;	
		}
		/**
		 * --iter
		 */
		iterator &operator--() {
			if (at == nullptr|| at->prev == nullptr|| at->prev->prev == nullptr) throw sjtu::invalid_iterator();
			at = at->prev;
			return *this; 
		}
		
		/**
		 * if the iter didn't point to a value
		 * throw " invalid"
		 */
		T &operator*() const { // 现在还在搞混..&返回的也是数据本身，但是作为引用返回
			if (at == nullptr || at->next == nullptr || at->prev == nullptr) {
				throw sjtu::invalid_iterator();
			}
			return at->data;
		}
		
		/**
		 * other operation
		 */
		T *operator->() const noexcept {
			return &(operator*()); // 复用
		}
		bool operator==(const iterator &rhs) const {
			return (at == rhs.at);
		}
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
	};
	/**
	 * return an iterator to the beginning
	 */
	iterator begin() const {
		return iterator(head_->next);
	} 
	/**
	 * return an iterator to the ending
	 * in fact, it returns the iterator point to nothing,
	 * just after the last element.
	 */
	iterator end() const {
		return iterator(tail_);
	}
	/**
	 * if the iter didn't point to anything, do nothing,
	 * otherwise, delete the element pointed by the iter
	 * and return the iterator point at the same "index"
	 * e.g.
	 * 	if the origin iterator point at the 2nd element
	 * 	the returned iterator also point at the
	 *  2nd element of the list after the operation
	 *  or nothing if the list after the operation
	 *  don't contain 2nd elememt.
	 */
	iterator erase(iterator pos) {
		if (!pos.at || pos == end()) throw sjtu::invalid_iterator();
		node* ptr = pos.at;
		node* ret = ptr->next;
		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
		delete(ptr);
		size--;
		return iterator(ret);
	}

	/**
	 * the following are operations of double list
	 */
	void insert_head(const T &val) {
		node* nd = new node(val, head_, head_->next);
		head_->next = nd;
		nd->next->prev = nd;
		size++;
	}
	void insert_tail(const T &val) {
		node* nd = new node(val, tail_->prev, tail_);
		tail_->prev = nd;
		nd->prev->next = nd;
		size++;
	}
	void delete_head() {
		if (!size) throw sjtu::container_is_empty();
		node* target = head_->next;
		head_->next = target->next;
		target->next->prev = head_;
		delete(target);
		size--;
	}
	void delete_tail() {
		if (!size) throw sjtu::container_is_empty();
		node* target = tail_->prev;
		tail_->prev = target->prev;
		target->prev->next = tail_;
		delete(target);	
		size--;	
	}
	/**
	 * if didn't contain anything, return true,
	 * otherwise false.
	 */
	bool empty() {
		return !size;
	}
};

template <class T> class deque {
public:
	static const int M = 256;
	int tt_size = 0;

	struct circ{
		T arr[M];
		int head = 0;
		int size = 0;
		T &query(int i) {
			return arr[(head + i) % M]; 
		}
		const T &query(int i) const {
			return arr[(head + i) % M];
		}
	};

	double_list<circ*> data;

	class const_iterator;
	class iterator {
	private:
		// 指示当前所在的数组
		typename double_list<circ*>::iterator it = double_list<circ*>().end();
		int local_ptr = -1;
		int global_ptr = -1;
		deque<T>* vec = nullptr;

	public:
	iterator(typename double_list<circ*>::iterator otit, int lp, int gp, deque<T>* dptr): 
		it(otit), local_ptr(lp), global_ptr(gp), vec(dptr) {}
	/**
	* return a new iterator which points to the n-next element.
	* if there are not enough elements, the behaviour is undefined.
	* same for operator-.
	*/
	iterator operator+(const int &n) const {
		// // TODO
		// int left = n;
		// typename double_list<circ*>::iterator curr = it;
		// if (local_sz - local_ptr > n) {
		// 	T* out = &(*it->query(local_ptr + n));
		// 	return iterator(it, local_ptr + n, local_sz, global_ptr + n, out);
		// }
		// while (left > (*curr)->size) {

		// }
	}
	iterator operator-(const int &n) const {}

	/**
	* return the distance between two iterators.
	* if they point to different vectors, throw
	* invaild_iterator.
	*/
	int operator-(const iterator &rhs) const {
		if (vec != rhs.vec) throw sjtu::invalid_iterator();
		return global_ptr - rhs.global_ptr;
	}

	iterator &operator+=(const int &n) {
		*this = *this + n;
		return *this;
	}
	iterator &operator-=(const int &n) {
		*this = *this - n;
		return *this;
	}

	/**
	* iter++
	*/
	iterator operator++(int) {
		iterator tmp = *this;
		if (local_ptr < (*it)->size - 1) {
			local_ptr++;
		} else {
			it++;
			local_ptr = 0;
		}
		global_ptr++; 
		return tmp;
	}
	/**
	* ++iter
	*/
	iterator &operator++() {
		if (local_ptr < (*it)->size - 1) {
			local_ptr++;
		} else {
			it++;
			local_ptr = 0;
		}
		global_ptr++; 
		return *this;
	}
	/**
	* iter--
	*/
	iterator operator--(int) {
		iterator tmp = *this;
		if (local_ptr > 0) {
			local_ptr--;
		} else {
			it--;
			local_ptr = (*it)->size - 1;
		}
		global_ptr--; 
		return tmp;
	}
	/**
	* --iter
	*/
	iterator &operator--() {
		if (local_ptr > 0) {
			local_ptr--;
		} else {
			it--;
			local_ptr = (*it)->size - 1;
		}
		global_ptr--; 
		return *this;
	}

	/**
	* *it
	*/
	T &operator*() const {
		if (global_ptr < 0 || global_ptr >= vec->tt_size) throw sjtu::invalid_iterator();
		return (*it)->query(local_ptr);
	}
	/**
	* it->field
	*/
	T *operator->() const noexcept {
		return &((*it)->query(local_ptr));
	}

	/**
	* check whether two iterators are the same (pointing to the same
	* memory).
	*/
	bool operator==(const iterator &rhs) const {
		if (vec != rhs.vec) return false;
		return global_ptr == rhs.global_ptr;
	}
	bool operator==(const const_iterator &rhs) const {
		if (vec != rhs.vec) return false;
		return global_ptr == rhs.global_ptr;
	}
	/**
	* some other operator for iterators.
	*/
	bool operator!=(const iterator &rhs) const {
		return ! (*this == rhs);
	}
	bool operator!=(const const_iterator &rhs) const {
		return ! (*this == rhs);
	}
	};

	class const_iterator {
		/**
		* it should has similar member method as iterator.
		* you can copy them, but with care!
		* and it should be able to be constructed from an iterator.
		*/
	};

	/**
		* constructors.
		*/
	deque() {}
	deque(const deque &other) {
		for (auto it = other.data.begin(); it != other.data.end(); it++) {
			data.push_back(new circ(*it));
		}
		tt_size = other.tt_size;
	}

	/**
		* deconstructor.
		*/
	~deque() {
		for (auto it = data.begin(); it != data.end(); it++) {
			delete *it;
		}
	}

	/**
		* assignment operator.
		*/
deque &operator=(const deque &other) {
    if (this == &other) {
        return *this;
    }
    for (auto it = data.begin(); it != data.end(); ++it) {
        delete *it; 
    }
    data.clear(); 
    for (auto it = other.data.begin(); it != other.data.end(); ++it) {
        circ* new_block = new circ(**it); 
        data.push_back(new_block);
    }
    tt_size = other.tt_size;
    return *this;
}

	/**
		* access a specified element with bound checking.
		* throw index_out_of_bound if out of bound.
		*/
	T &at(const size_t &pos) {
		if (pos >= tt_size) { // size_t 不需要考虑 < 0
			throw sjtu::index_out_of_bound();
		} 
		iterator ptr = begin();
		ptr += pos;
		return (*ptr);
	}

	const T &at(const size_t &pos) const {
		if (pos >= tt_size) { 
			throw sjtu::index_out_of_bound();
		} 
		const_iterator ptr = cbegin();
		ptr += pos;
		return (*ptr);
	}

	T &operator[](const size_t &pos) {
		if (pos >= tt_size) {
			throw sjtu::index_out_of_bound();
		} 
		iterator ptr = begin();
		ptr += pos;
		return (*ptr);
	}
	const T &operator[](const size_t &pos) const {
		if (pos >= tt_size) {
			throw sjtu::index_out_of_bound();
		} 
		const_iterator ptr = cbegin();
		ptr += pos;
		return (*ptr);
	}

	/**
		* access the first element.
		* throw container_is_empty when the container is empty.
		*/
	const T &front() const {
		if (tt_size == 0 || data.empty()) throw sjtu::container_is_empty();
		typename double_list<circ*>::iterator cit = data.begin();
		return (*cit)->query(0);
	}
	/**
		* access the last element.
		* throw container_is_empty when the container is empty.
		*/
	const T &back() const {
		if (tt_size == 0 || data.empty()) throw sjtu::container_is_empty();
		typename double_list<circ*>::iterator last_circ = data.end();
		last_circ--;
		int last_sz = (*last_circ)->size;
		return (*last_circ)->query(last_sz - 1);
	}

	/**
		* return an iterator to the beginning.
		*/
	iterator begin() {
		return iterator(data.begin(), 0, 0, this);
	}
	const_iterator cbegin() const {
		return const_iterator(data.begin(), 0, 0, this);
	}

	/**
		* return an iterator to the end.
		*/
	iterator end() {
		return iterator(data.end(), 0, tt_size, this);
	}
	const_iterator cend() const {
		return cosnt_iterator(data.end(), 0, tt_size, this);
	}

	/**
		* check whether the container is empty.
		*/
	bool empty() const {
		return (tt_size == 0);
	}

	/**
		* return the number of elements.
		*/
	size_t size() const {
		return tt_size;
	}

	/**
		* clear all contents.
		*/
	void clear() {
		for (auto it = data.begin(); it != data.end(); it++) {
			delete *it;
		}
		data.clear();
		tt_size = 0;
	}

	/**
		* insert value before pos.
		* return an iterator pointing to the inserted value.
		* throw if the iterator is invalid or it points to a wrong place.
		*/
	iterator insert(iterator pos, const T &value) {}

	/**
		* remove the element at pos.
		* return an iterator pointing to the following element. if pos points to
		* the last element, return end(). throw if the container is empty,
		* the iterator is invalid, or it points to a wrong place.
		*/
	iterator erase(iterator pos) {}

	/**
		* add an element to the end.
		*/
	void push_back(const T &value) {}

	/**
		* remove the last element.
		* throw when the container is empty.
		*/
	void pop_back() {}

	/**
		* insert an element to the beginning.
		*/
	void push_front(const T &value) {}

	/**
		* remove the first element.
		* throw when the container is empty.
		*/
	void pop_front() {}
};

} // namespace sjtu

#endif
