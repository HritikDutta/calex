#pragma once
#include <iostream>

//Custom stack implimentation

template<typename T>
class Stack
{
private:
	//Stack node
	class Node
	{
	public:
		T data;
		Node* next;

		Node(T value)
		: data(value), next(nullptr) {}
	}* topNode;

public:
	Stack()
	: topNode(nullptr) {}

	void push(T data)
	{
		Node* newNode = new Node(data);

		newNode->next = topNode;
		topNode = newNode;
	}

	bool empty()
	{
		return topNode == nullptr;
	}

	T& top()
	{
		return topNode->data;
	}

	void pop()
	{
		if (topNode == nullptr)
			return;

		Node* temp = topNode;
		topNode = topNode->next;

		delete temp;
	}

	void display()
	{
		Node* index = topNode;

		while (index)
		{
			std::cout << index->data << ", ";
			index = index->next;
		}

		//Remove the last comma
		if (topNode)
			std::cout << "\b\b ";
	}

	~Stack()
	{
		while(!empty())
		{
			pop();
		}
	}
};