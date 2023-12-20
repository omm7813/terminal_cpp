// Queue class definition protected by include guards
#ifndef QUEUE_H
#define QUEUE_H

#include <cstdlib>
#include <stdexcept>

//Exceptions Defenition
class QueueFull : public exception
{
	public:
		virtual const char* what() const throw()
		{ 
			return "Queue is Full!"; //Throw an error when called
		}
};

class QueueEmpty : public exception
{
	public:
		virtual const char* what() const throw()
		{
			return "Queue is Empty!"; //Throw an error when called
		}
};

// Template definition for Queue class
template <typename T>
class Queue {
private:
    T *array; // Pointer to the array used for storing queue elements
    int capacity; // Maximum number of elements the queue can hold
    int size; // Current number of elements in the queue
    int front; // Index of the front element in the queue
    int rear; // Index to the position where the next element will be inserted

public:
    // Constructor with default capacity set to 10
    Queue(int capacity = 10);
    // Destructor to deallocate memory used by the queue
    ~Queue();
    // Function to add an element to the queue
    void enqueue(T element);
    // Function to remove an element from the queue
    void dequeue();
    // Function to check if the queue is empty
    bool isEmpty() const;
    // Function to check if the queue is full
    bool isFull() const;
    // Function to get the front element of the queue
    T front_element() const;
};

// Constructor implementation
template <typename T>
Queue<T>::Queue(int cap) : capacity(cap), size(0), front(0), rear(0) {
    array = new T[capacity]; // Allocating memory for the queue
}

// Destructor implementation
template <typename T>
Queue<T>::~Queue() {
    delete[] array; // Deallocating the array used for the queue
}

// enqueue() implementation
template <typename T>
void Queue<T>::enqueue(T element) {
    // Check if the queue is already full
    if (isFull()) {
        // Throw a QueueFull exception if there's no space left
        throw QueueFull();
    } else {
        array[rear] = element; // Inserting the element at the rear
        rear = (rear + 1) % capacity; // Updating rear position
        size++; // Increasing the size of the queue
    }
}

// dequeue() implementation
template <typename T>
void Queue<T>::dequeue() {
    // Check if the queue is empty
    if (isEmpty()) {
        // Throw a QueueEmpty exception if there are no elements left
        throw QueueEmpty();
    } else {
        front = (front + 1) % capacity; // Updating the front position
        size--; // Decreasing the size of the queue
    }
}

// isEmpty() implementation
template <typename T>
bool Queue<T>::isEmpty() const {
    return size == 0; // Returns true if size is 0
}

// isFull() implementation
template <typename T>
bool Queue<T>::isFull() const {
    return size == capacity; // Returns true if size equals capacity
}

// front_element() implementation
template <typename T>
T Queue<T>::front_element() const {
    return array[front]; // Returns the front element
}

#endif // QUEUE_H
