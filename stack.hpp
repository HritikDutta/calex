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
	}* head;

public:
	Stack()
	: head(nullptr) {}

	void push(T data)
	{
		Node* newNode = new Node(data);

		newNode->next = head;
		head = newNode;
	}

	bool empty()
	{
		return head == nullptr;
	}

	T& top()
	{
		return head->data;
	}

	void pop()
	{
		if (head == nullptr)
			return;

		Node* temp = head;
		head = head->next;

		delete temp;
	}

	void display()
	{
		Node* index = head;

		while (index)
		{
			std::cout << index->data << ", ";
			index = index->next;
		}

		//Remove the last comma
		if (head)
			std::cout << "\b\b ";
	}

	int size()
	{
		int cnt = 0;
		Node* temp = head;
		while (!temp)
		{
			cnt++;
			temp = temp->next;
		}

		return cnt;
	}

	void clean()
	{
		while (!empty())
			pop();
	}

	~Stack()
	{
		while(!empty())
			pop();
	}
};