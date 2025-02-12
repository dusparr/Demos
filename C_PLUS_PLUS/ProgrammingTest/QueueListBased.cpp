

#include "QueueListbased.h"
#include <cstring>  // memset


#pragma region Defines
#define TOTAL_MEM_SIZE  2048 
#define QUEUE_HEADER_SIZE  4
#define MAX_NUM_QUEUES  64
#define QUEUE_HEADER_SPACE  1 // offset at which the Q headers are stored
#define STORAGE_UNIT_BYTES 15 //The number of bytes in a storage unit
#pragma endregion Magic numbers
#pragma region Definitions
typedef unsigned char StorageUnitHandle;

///Structure of Memory used to keep track of positition in the Circular List 
///and to keep track of the data
struct StorageUnit
{
	unsigned char UnitPosition; //Circular
	unsigned char Units[STORAGE_UNIT_BYTES];
};
#pragma endregion The Structural Definitions
#pragma region DataMembers
//making these const means that in worst compilation scenarios, they are still only defined once.
const int QUEUE_SPACE_OFFSET =  QUEUE_HEADER_SIZE * MAX_NUM_QUEUES + QUEUE_HEADER_SPACE;
const int Total_Number_Of_Units =
(TOTAL_MEM_SIZE - QUEUE_HEADER_SIZE * MAX_NUM_QUEUES - 1) /
sizeof(StorageUnit);
///Organized like so:
///index       purpose
///--------------------------------------
///0           freelist handle
///1-192       all 64 available Q structs
///193-208     storage Unit 0
///209-224     storage Unit 1
///...         ...
///2017-2032   storage Unit 114
///2033-2047   unused
unsigned char data[TOTAL_MEM_SIZE];
#pragma endregion All of the data Members
#pragma region HelperFunctions
///HELPER
///simple access to the Free List
StorageUnitHandle& Get_Free_List()
{
	return data[0];
}
///HELPER
///TransLates handle Position to Data and returns The StorageUnit if possible
StorageUnit* GetStorageFromHandle(StorageUnitHandle handle)
{
	if (handle != NULL)
	{
		return reinterpret_cast<StorageUnit*>(data + QUEUE_SPACE_OFFSET + sizeof(StorageUnit)*(handle - 1));
	}
	else
	{
		return NULL;
	}

}
///gives us the next Handle from the Free List
StorageUnitHandle GetUnitFromFree()
{
	StorageUnitHandle allocated = Get_Free_List();
	if (allocated == NULL)
	{
		on_out_of_memory();
	}
	else
	{
		Get_Free_List() = GetStorageFromHandle(allocated)->UnitPosition;
		GetStorageFromHandle(allocated)->UnitPosition = 0; // Just in case
	}
	return allocated;

}
///HELPER
///Return block to the void
void DeallocateUnit(StorageUnitHandle unit)
{
	// get the free list and add this unit to the front;
	GetStorageFromHandle(unit)->UnitPosition = Get_Free_List();
	Get_Free_List() = unit;
}
void InitializeFreeList()
{
	StorageUnitHandle block = 1;
	while (block <= Total_Number_Of_Units)
	{
		GetStorageFromHandle(block)->UnitPosition = block + 1;
		++block;
	}
}
void GetFirstUnusedQueueHeader(Q * &newQueue)
{
	for (int queueIndex = 0; queueIndex < MAX_NUM_QUEUES; ++queueIndex)
	{
		Q* queueHeader = reinterpret_cast<Q*>(data + QUEUE_HEADER_SPACE + queueIndex * QUEUE_HEADER_SIZE);
		if (!queueHeader->InUse)
		{
			newQueue = queueHeader;
			break;
		}
	}
}
///Helper function to encapsulate the intialization of a Queue
void InitializeQueue(Q * Initializer)
{

	Initializer->InUse = 1;
	// Set flag for use
	Initializer->HeadUnit = GetUnitFromFree(); // get the first unit for the queue
	Initializer->TailUnit = Initializer->HeadUnit; //set all of the units to the same
	Initializer->GrowthPos = Initializer->HeadUnit;
	GetStorageFromHandle(Initializer->HeadUnit)->UnitPosition =
		Initializer->HeadUnit;
	Initializer->HeadOffset = Initializer->TailOffset = 0;
}
void CheckForWrapping(unsigned int newOffset, Q * q, bool &updateGrowth)
{
	//IF new unit is before next unit, then we have circuled around
	if (newOffset < q->TailOffset)
	{
		q->TailUnit = q->HeadUnit;
		updateGrowth = true;
	}
}
void HandleFullUnit(unsigned int newOffset, Q * q, bool &updateGrowth, const StorageUnitHandle &oldTailUnit)
{

	StorageUnitHandle newUnitHandle = GetUnitFromFree(); //get a new Unit handle
	StorageUnit* newUnit = GetStorageFromHandle(newUnitHandle); //get the Unit itself
																  // Copy trailing elements to new growth

	for (unsigned char copyOffset = 0; copyOffset < newOffset; ++copyOffset)
	{
		newUnit->Units[copyOffset] = GetStorageFromHandle(q->HeadUnit)->Units[copyOffset];
	}
	//if we have new growth

	if (updateGrowth)
	{
		//old tails becomes new growth
		q->GrowthPos = oldTailUnit;
	}

#ifdef SAFE
	if (newUnit == NULL)
	{
		// we have run out of space in which to put data
		on_out_of_memory(); /// we knew about this earlier, when the Get Unit from free called it;
		///but I am saving the program from cascading failure (if that is not intended, #Define SAFE_MEMORY should be undeffed
	}

	else
	{
#endif // SAFE
		//connect new Unit to Queue
		newUnit->UnitPosition = q->HeadUnit;
		GetStorageFromHandle(q->GrowthPos)->UnitPosition = newUnitHandle;
		q->TailUnit = newUnitHandle;
		updateGrowth = false;
#ifdef SAFE
	}
#endif // SAFE
}
#pragma endregion All of the Helper Functions
#pragma region Main Functions
///INITIALIZER
///SETUP THE MEMORY SPACE
void InitializeQueueSpace()
{
	memset(data, 0, QUEUE_HEADER_SIZE*MAX_NUM_QUEUES); //set all of the QueueData to 0
	data[0] = 1;  //freelist points to Unit 1
	InitializeFreeList();
	// make sure final Unit points to Null
	GetStorageFromHandle(Total_Number_Of_Units)->UnitPosition = NULL;
}
///Create New Queue
///Success: Queue is returned 
///FAILURE: 65 Queues are created, or all memory is allocated
Q* create_queue()
{
	Q* newQueue = NULL;
	// Find an unused Queue Header

	GetFirstUnusedQueueHeader(newQueue);

	if (newQueue != NULL)
	{
		InitializeQueue(newQueue);
	}

	else

	{
		// Too many Queues
		on_illegal_operation();
	}
	return newQueue;
}
///MAIN
///return a Queue to the free list
void destroy_queue(Q* deadQueue)
{
	//Flag the queue for disuse
	deadQueue->InUse = 0;
	StorageUnitHandle unitAfterGrowth = GetStorageFromHandle(deadQueue->GrowthPos)->UnitPosition;
	GetStorageFromHandle(deadQueue->GrowthPos)->UnitPosition = Get_Free_List();
	Get_Free_List() = unitAfterGrowth;
}

void enqueue_byte(Q* q, unsigned char b)
{
	StorageUnit* tailUnit = GetStorageFromHandle(q->TailUnit);
	tailUnit->Units[q->TailOffset] = b;
	// If the tail wraps to the head unit but doesn't collide,

	// move the Growth unit to where the tail unit used to be

	StorageUnitHandle oldTailUnit = q->TailUnit;

	bool updateGrowth = false;
	unsigned int newOffset = (q->TailOffset + 1) % STORAGE_UNIT_BYTES;
	CheckForWrapping(newOffset, q, updateGrowth);
	q->TailOffset = newOffset;
	//If we haven't moved the head
	if (q->TailUnit == q->HeadUnit)
	{
		//check if it is filled
		if (newOffset == q->HeadOffset)
		{
			HandleFullUnit(newOffset, q, updateGrowth, oldTailUnit);
		}
	}
	// growth must be known for constant time insertion
	if (updateGrowth)
	{
		q->GrowthPos = oldTailUnit;
	}
}
///MAIN
///Return the head value of the Queue (the first object put in) and move to the next
///deallocate if needed.
unsigned char dequeue_byte(Q* q)
{
	//check if Queue points to itself and has no offset differences, that means its empty
	if (q->HeadUnit == q->TailUnit && q->HeadOffset == q->TailOffset)
	{
		on_illegal_operation();
		///unsure if should return; program does not crash, but is unknown what will occur
	}
	//get the head byte
	unsigned char ReturnValue = GetStorageFromHandle(q->HeadUnit)->Units[q->HeadOffset];
	unsigned int newOffset = (q->HeadOffset + 1) % STORAGE_UNIT_BYTES;
	// If we wrapped around check for 

	if (newOffset < q->HeadOffset)
	{
		StorageUnitHandle newHeadUnit = GetStorageFromHandle(q->HeadUnit)->UnitPosition;
		// If the Unit we came from a unit other than the tail Unit
		if (q->HeadUnit != q->TailUnit)
		{
			//it is an unused Unit and can be deallocated at will.
			GetStorageFromHandle(q->TailUnit)->UnitPosition = newHeadUnit;
			DeallocateUnit(q->HeadUnit);
		}
		q->HeadUnit = newHeadUnit;
	}
	q->HeadOffset = newOffset;
	return ReturnValue;

}
#pragma endregion Functions called from outside this file