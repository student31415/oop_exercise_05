#pragma once

#include <memory>

template<class T> class List{
	private:
		struct Element{
			T data;
			std::shared_ptr<Element> next;
			std::weak_ptr<Element> prev;
			Element() {}
			Element(T d) : data(d) {}
			Element(T d, std::shared_ptr<Element> n) : data(d), next(n) {}
			Element(T d, std::shared_ptr<Element> n, std::shared_ptr<Element> p) : data(d), next(n), prev(p) {}
			Element(T d, std::shared_ptr<Element> n, std::weak_ptr<Element> p) : data(d), next(n), prev(p) {}
		};
		std::shared_ptr<Element> head, tail;
	public:
		class iterator{
			private:
				friend class List; 
				std::weak_ptr<List::Element> ukazatel;
			public:
				using difference_type = int;
				using value_type = T;
				using reference = T&;
				using pointer = T*;
				using iterator_category = std::forward_iterator_tag;
				reference operator*(){
					if (!ukazatel.lock()->next) 
						throw std::out_of_range("attempt to access unexisting element value");
					return ukazatel.lock()->data;
				}
				pointer operator->(){
					if (!ukazatel.lock()->next) 
						throw std::out_of_range("attempt to access unexisting element");
					return &(ukazatel.lock()->data);
				}
				iterator& operator++(){
					if (!ukazatel.lock()->next) 
						throw std::out_of_range("attempt to move further than end()");
					ukazatel = (*ukazatel.lock()).next;
					return *this;
				}
				bool operator!=(const iterator& other){
					return ukazatel.lock() != other.ukazatel.lock();
				}
		};
		friend class iterator;
		List(){
			tail = std::make_shared<Element>(Element());
			head = tail;
		}
		~List(){}
	
		iterator begin(){
			iterator i;
			i.ukazatel = head;
			return i;
		}
		iterator end(){
			iterator i;
			i.ukazatel = tail;
			return i;
		}
	
		bool empty()const{ 
			return head==tail;
		}
		size_t size()const{
			size_t size = 0;
			for(std::weak_ptr<Element> W(head); W.lock() !=tail; W=W.lock()->next, ++size);
			return size;
		}
		void clear(){
			head = tail;
			return;
		}
		void insert(iterator i, const T &val){
			if (!(i.ukazatel.lock()->prev.lock())){
				head = std::make_shared<Element>(Element(val, head));
				head->next->prev = head;
			}else{
				std::shared_ptr<Element> el = std::make_shared<Element>
						(Element(val, i.ukazatel.lock(), i.ukazatel.lock()->prev));
				el->prev.lock()->next = el;
				el->next->prev = el;
			}
		}
		iterator erase(iterator i){
			if (i.ukazatel.lock() == tail) 
				throw std::out_of_range("attempt to remove end()");
			iterator ret = i;
			++ret;
			if(!(i.ukazatel.lock()->prev.lock())){
				head->next->prev = std::weak_ptr<Element>();
				head = head->next;
			}else{
				std::shared_ptr<List<T>::Element> el = i.ukazatel.lock();
				el->next->prev = el->prev;
				el->prev.lock()->next = el->next;
			}
			return ret;
		}
		T& operator[](int i){
			return *std::next(begin(), i); 
		}
};
