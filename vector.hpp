#ifndef VECTOR_H
#define VECTOR_H

#include<cstdlib>
#include <stdexcept>
#include "vfs.hpp"
using namespace std;

template <typename T>
class Vector
{
	private:
		int v_size;        // current size of vector (number of elements in vector)
    	int v_capacity;    // capacity of vector
    	T* data;           // pointer to int(array) to store elements
	public:
		Vector(int cap=0);			//Constructor
		~Vector();					//Destructor
		int size() const;				//Return current size of vector
		int capacity() const;			//Return capacity of vector
		bool empty() const; 			//Rturn true if the vector is empty, False otherwise
		const T& front();				//Returns reference of the first element in the vector
		const T& back();				//Returns reference of the Last element in the vector
		void push_back(T element);		//Add an element at the end of vector
		void insert(int index, T element); //Add an element at the index 
		void erase(int index);			//Removes an element from the index
		T& operator[](int index);			//Returns the reference of an element at given index
		T& at(int index); 				//return reference of the element at given index
		void shrink_to_fit();			//Reduce vector capacity to fit its size
		void display();

        class Iterator {
        private:
            T* ptr;

        public:
            Iterator(T* p) : ptr(p) {}

            // Overload the increment operator (++)
            Iterator& operator++() {
                ++ptr;
                return *this;
            }

            // Overload the increment operator (--)
            Iterator& operator--() {
                --ptr;
                return *this;
            }

            // Overload the not-equal operator (!=) to compare iterator with another iterator
            bool operator!=(const Iterator& other) const {
                return ptr != other.ptr;
            }

            // Overload the not-equal operator (!=) to compare iterator with another iterator
            bool operator==(const Iterator& other) const {
                return ptr == other.ptr;
            }

            // Overload the dereference operator (*) to access the value
            T& operator*() const {
                return *ptr;
            }
            // Overload the addition operator to add an integer to an iterator
            Iterator operator+(int value) {
                return Iterator(ptr + value);
            }

            // Overload the subtraction operator to subtract an integer from an iterator
            Iterator operator-(int value) {
                return Iterator(ptr - value);
            }
        };

        // Function to get the iterator to the beginning of the vector
        Iterator begin() {  
            return Iterator(data);
        }

        // Function to get the iterator to the end of the vector (one past the last element)
        Iterator end() {
            return Iterator(data + v_size);
        }
        
};

// Constructor: Initializes a MyVector object with the specified capacity.
template <typename T>
Vector<T>::Vector(int cap) : v_size(0), v_capacity(cap), data(new T[cap]) {}

// Destructor: Cleans up memory allocated for the vector.
template <typename T>
Vector<T>::~Vector() {
    delete[] data;  // Release the allocated memory.
}

// Provides direct access to elements.
template <typename T>
T& Vector<T>::operator[ ](int i) {
    return data[i]; 
}

// Appends an element to the end of the vector.
template <typename T>
void Vector<T>::push_back(T element) {
    if (v_size >= v_capacity) {
        // Increase capacity if necessary, by doubling it or setting it to 1 if it was 0.
        int newCapacity = (v_capacity == 0) ? 1 : (v_capacity * 2);
        T* newdata = new T[newCapacity];

        // Copy existing elements to the new array.
        for (int i = 0; i < v_size; ++i) {
            newdata[i] = data[i];
        }

        data = newdata;
        v_capacity = newCapacity; // Update capacity.
    }
    // Add the new element.
    data[v_size++] = element;
}

// Inserts an element at a given index.
template <typename T>
void Vector<T>::insert(int index, T element) {
    if (index > v_size || index < 0) {
        throw out_of_range("Invalid index.");
    }

    // Check if a resize is needed.
    if (v_size >= v_capacity) {

		if (v_capacity == 0) {
			v_capacity = 1;
		} else {
			v_capacity = v_capacity*2;
		}
		T* newdata = new T[v_capacity];

        // Copy elements to new memory space.
        for (int i = 0; i < v_size; ++i) {
            newdata[i] = data[i];
        }

        delete[] data;  // Free old memory.
        data = newdata; // Point to the new memory.
	}
	// Shift all elements from 'index' to the end one position to the right.
    for (int i = v_size; i > index; --i) {
        data[i] = data[i-1];
    }

    // Insert the new element at the given index.
    data[index] = element;
    v_size++;  // Increase the size of the vector.
}

template <typename T>
void Vector<T>::erase(int index) {
    if (index < 0 || index >= v_size) {
        throw std::out_of_range("Index out of range");
    }

    // If the vector holds pointers, delete the object pointed to

    // Shift elements down to fill the gap.
    for (int i = index; i < v_size - 1; ++i) {
        data[i] = data[i + 1];
    }
    
    // Reduce the size of the vector.
    v_size--;
}

// Accesses the element at a given index with bounds-checking.
template <typename T>
T& Vector<T>::at(int index) {
    if (index >= v_size || index < 0) {
        throw out_of_range("Invalid index.");
    }

    return data[index];
}

// Returns the first element.
template <typename T>
const T& Vector<T>::front() {
    if (empty()) {
        throw out_of_range("Vector is empty.");
    }

    return data[0];
}

// Returns the last element.
template <typename T>
const T& Vector<T>::back() {
    if (empty()) {
        throw out_of_range("Vector is empty.");
    }

    return data[v_size - 1];
}

// Returns the number of elements.
template <typename T>
int Vector<T>::size() const {
    return v_size;
}

// Returns the vector's capacity.
template <typename T>
int Vector<T>::capacity() const {
    return v_capacity;
}

// Reduces the capacity of the vector to fit its size.
template <typename T>
void Vector<T>::shrink_to_fit() {
    if (v_size < v_capacity) {
        T* newData = new T[v_size];

        // Copy elements to the new memory block.
        for (int i = 0; i < v_size; ++i) {
            newData[i] = data[i];
        }

        delete[] data;  // Free old memory.
        data = newData; // Point to the new memory block.
        v_capacity = v_size;
    }
}

// Checks if the vector is empty.
template <typename T>
bool Vector<T>::empty() const {
    return v_size == 0;
}

#endif