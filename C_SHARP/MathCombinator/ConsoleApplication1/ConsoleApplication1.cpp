// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stack>



static void FizzBuzz(unsigned int num)
{
	
	if (num == 0)
	{
		std::cout << num << "\n";
		//Console.WriteLine(num);
		return;
	}
	bool notFB = true;
	if (num % 3 == 0)
	{
		notFB = false;
		std::cout << "Fizz";
		///Console.Write("Fizz");
	}
	if (num % 5 == 0)
	{
		notFB = false;
		std::cout << "Buzz";
		//Console.Write("Buzz");
	}
	if (notFB)
	{
		std::cout << num;
		//Console.Write(num);
	}
	std::cout << "\n";
	//int i[4];
	//int * i = new int[10];
	//std::unique_ptr<int *> iU = std::make_unique<int*>(i);
}


typedef struct Node {
    struct Node* next;
    struct Node* prev;
    struct Node* child;
    char value;
} Node;

void print(Node* h)
{
    while (h)
    {
        std::cout << h->value;
        h = h->next;
    }
}

Node* Flatten(Node* Head)
{
    if (!Head)
        return 0;

    std::stack<Node*> stackONodes;
    stackONodes.push(Head);
    Node* out = Head;
    Node* Cur;
    while (stackONodes.size())
    {

        Cur = stackONodes.top();
        stackONodes.pop();
        if (Cur != nullptr)
        {
            if (Cur->next)
            {
                stackONodes.push(Cur->next);
            }
            if (Cur->child)
            {
                stackONodes.push(Cur->child);
            }
            if (stackONodes.top())
            {
                Cur->next = stackONodes.top();
            }
            Cur->child = 0;
        }
    }
    //Cur->next = 0;
    return out;
}



void InsertAtEnd(Node*& head, Node*& tail, Node* newNode)
{
    newNode->next = nullptr;
    newNode->prev = nullptr;

    if (!head)
    {
        head = newNode;
        tail = newNode;
    }
    else
    {
        tail->next = newNode;
        newNode->prev = tail;
    }
}

Node* Flatten2(Node* head)
{
    if (!head)
        return 0;

    std::stack<Node*> stackONodes;
    stackONodes.push(head);

    Node* head2 = nullptr;
    Node* tail2 = nullptr;

    while (stackONodes.size())
    {

        Node* Cur = stackONodes.top();
        stackONodes.pop();

        if (Cur)
        {
            if (Cur->next)
            {
                stackONodes.push(Cur->next);
            }
            if (Cur->child)
            {
                stackONodes.push(Cur->child);
            }
            InsertAtEnd(head2, tail2, Cur);
        }
    }
    return head2;
}



void TestData()
{
    Node a{ nullptr, nullptr, nullptr, 'a' };
    Node b{ nullptr, nullptr, nullptr, 'b' };
    Node c{ nullptr, nullptr, nullptr, 'c' };
    Node d{ nullptr, nullptr, nullptr, 'd' };
    Node e{ nullptr, nullptr, nullptr, 'e' };
    Node o1{ nullptr, nullptr, nullptr, '1' };
    Node o2{ nullptr, nullptr, nullptr, '2' };
    Node o3{ nullptr, nullptr, nullptr, '3' };
    Node k{ nullptr, nullptr, nullptr, 'k' };
    Node f{ nullptr, nullptr, nullptr, 'f' };
    Node o6{ nullptr, nullptr, nullptr, '6' };



    a.next = &b;
    b.next = &c;
    c.next = &d;
    d.next = &e;

    b.prev = &a;
    c.prev = &b;
    d.prev = &c;
    e.prev = &d;

    c.child = &o1;
    d.child = &o6;

    o1.next = &o2;
    o2.next = &o3;

    o3.prev = &o2;
    o2.prev = &o1;

    o3.child = &k;

    k.next = &f;
    f.prev = &k;

    //Node * n = Flatten(&a);
    //print(&a);
    //print(&o1);
    //print(&k);
    //print(n);

    Node* n = Flatten(&a);
    print(n);

}


int main() {
    // you can write to stdout for debugging purposes, e.g.
    std::cout << "This is a debug message" << std::endl;
    TestData();

    return 0;
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
