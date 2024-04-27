#pragma once
#pragma region LogicalDefines
///practicing c style definition saftey
//SAFE makes the program not crash after a Out_Of_Memory call, when it attempts to hook up a "new" node; 
//normally this fails (Null ptr exception) but with SAFE on, it just pushes Memory error again
//THIS IS ONLY USEFULL FOR DEBUGGING where you may want to play with reseting the Queues after memory error
//and not crashing 
#ifndef SAFE
//#define SAFE 
#endif // !SAFE
#pragma endregion Defines that change Code Logic
#pragma region Helper Definitions
typedef unsigned int BitMap;

class Q
{
public:
	///Using an BitMap to keep track of the following in the Queue
	/*
	0|1 -> allocated, unallocated
	0000000 ... 1111111 | 0...127 -> handle to storageblock head
	0000 ... 1111 | 0...15 -> offset of head within that block
	0000000 ... 1111111 | 0...127 -> handle to storageblock tail
	0000 ... 1111 | 0...15 -> offset of tail within that block
	0000000 ... 1111111 | 0...127 -> handle to Growth Block 
	*/
	BitMap unused     : 2;
	BitMap InUse      : 1;  
	BitMap HeadUnit   : 7;  
	BitMap HeadOffset : 4; 
	BitMap TailUnit   : 7; 
	BitMap TailOffset : 4; 
	BitMap GrowthPos  : 7;  
};
#pragma endregion
#pragma region Function Definitions


// Must be called before any of the other queue functions
void InitializeQueueSpace();



// Creates a FIFO byte queue, returning a handle to it.
Q * create_queue();
// Destroy an earlier created byte queue.
void destroy_queue(Q * q);
// Adds a new byte to a queue.
void enqueue_byte(Q * q, unsigned char b);
// Pops the next byte off the FIFO queue.
unsigned char dequeue_byte(Q * q);
extern void on_out_of_memory() ;
extern void on_illegal_operation() ;

#pragma endregion Usable functions



