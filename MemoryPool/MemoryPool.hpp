#pragma once


#include <stdexcept>
#include <iostream>
#include <malloc.h>

class MemoryPool
{
	typedef unsigned char uchar;
	typedef unsigned int uint;



public:

	template<typename T>
	class MemoryWrapper
	{
		friend class MemoryPool;

	public:
		MemoryWrapper(T* l_rawPtrToBlock) : m_rawPtrToBlock(l_rawPtrToBlock) {}

		MemoryWrapper(const MemoryWrapper& l_memoryWrapper) = default;
		MemoryWrapper& operator=(const MemoryWrapper& l_memoryWrapper) = default;

		MemoryWrapper(MemoryWrapper&& l_memoryWrapper) noexcept = default;

		void WriteToBlock(const T& m_srcData)
		{
			if (nullptr == m_rawPtrToBlock) {
				std::cerr << "Attempting to write to a null pointer." << std::endl;
				exit(EXIT_FAILURE);
			}
			
			memcpy((void*)m_rawPtrToBlock, (void*)&m_srcData, sizeof(T));
			
		}

#ifdef _DEBUG

		unsigned int GetIndexOfBlock()
		{
			if (nullptr != m_rawPtrToBlock) {
				uint* lv_temp = reinterpret_cast<uint*>(m_rawPtrToBlock);
				return *lv_temp;
			}
		}

		T GetValuePtrToBlock()
		{
			if (nullptr != m_rawPtrToBlock) {
				return *m_rawPtrToBlock;
			}
		}
#endif


	private:
		T* m_rawPtrToBlock{};
	};

public:

	MemoryPool(const unsigned int l_sizeOfEachBlock, const unsigned int l_totalNumBlocks)
		:m_sizeOfEachBlock(l_sizeOfEachBlock), m_totalNumBlocks(l_totalNumBlocks),
		m_totalNumFreeBlocks(l_totalNumBlocks), m_totalNumAllocatedBlocks(0),
		m_indexOfNextFreeBlock(0), m_sizeOfFreeBlocksList(1)
	{
		if (l_sizeOfEachBlock < sizeof(uint)) {

			/*We require at least sizeof(uint) for each block size since we are going to store the index of the next
			* free block of memory in the blocks themselves so we have to make sure each block can store uint types
			*/
			throw std::runtime_error{ "Requested size of each block needs to be at least the size of the unsigned int." };
		}

		if (0 == l_totalNumBlocks) {
			throw std::runtime_error{"Total number of blocks cannot be 0."};
		}

		m_startOfMemoryPool = reinterpret_cast<uchar*>(malloc(l_sizeOfEachBlock * l_totalNumBlocks));

		if (nullptr == m_startOfMemoryPool) {
			throw std::runtime_error{"malloc() failed to allocate requested of memory."};
		}

		m_nextFreeMemoryBlock = m_startOfMemoryPool;
	}

	MemoryPool(const MemoryPool&) = delete;
	void operator=(const MemoryPool&) = delete;

	template<typename T>
	MemoryWrapper<T> AllocateNewBlock()
	{
		void* lv_blockToAllocate{};

		if (m_sizeOfEachBlock < sizeof(T)) {
			std::cerr << "Allocation failed: The size of the type exceeds the size of each block in the pool." << std::endl;
		}
		else {
			if (0 < m_totalNumFreeBlocks) {

				if (1 < m_sizeOfFreeBlocksList) {
					--m_sizeOfFreeBlocksList;
					uint* lv_temp = reinterpret_cast<uint*>(m_nextFreeMemoryBlock);
					lv_blockToAllocate = reinterpret_cast<void*>(m_nextFreeMemoryBlock);

					if(m_totalNumBlocks == *lv_temp) {
						m_nextFreeMemoryBlock = nullptr;
						m_sizeOfFreeBlocksList = 0;
						m_indexOfNextFreeBlock = 0;
					}
					else {
						m_nextFreeMemoryBlock = FindAddressNextFreeBlockFromIndex(*(reinterpret_cast<uint*>(m_nextFreeMemoryBlock)));
					}
				}

				else {
					uint* lv_temp = reinterpret_cast<uint*>(m_nextFreeMemoryBlock);

					++m_indexOfNextFreeBlock;
					*lv_temp = m_indexOfNextFreeBlock;
					lv_blockToAllocate = reinterpret_cast<void*>(m_nextFreeMemoryBlock);

					if (m_indexOfNextFreeBlock < m_totalNumBlocks) {
						m_nextFreeMemoryBlock = FindAddressNextFreeBlockFromIndex(m_indexOfNextFreeBlock);
					}
					else {
						m_nextFreeMemoryBlock = nullptr;
						m_sizeOfFreeBlocksList = 0;
						m_indexOfNextFreeBlock = 0;
					}

				}
			
				--m_totalNumFreeBlocks;
				++m_totalNumAllocatedBlocks;
			}
		}

		return MemoryWrapper<T>{ reinterpret_cast<T*>(lv_blockToAllocate)};
	}



	template<typename T>
	void DeallocateBlock(MemoryWrapper<T>& l_blockToDeallocate)
	{
		
		CheckAddressRange((void*)l_blockToDeallocate.m_rawPtrToBlock);

		if (nullptr == l_blockToDeallocate.m_rawPtrToBlock || true == IsAddressInFreeListBlocks((void*)l_blockToDeallocate.m_rawPtrToBlock)) {
			return;
		}

		uint lv_index{};
		if (nullptr != m_nextFreeMemoryBlock) {
			lv_index = FindIndexFromAddress(m_nextFreeMemoryBlock);
			m_nextFreeMemoryBlock = reinterpret_cast<uchar*>(l_blockToDeallocate.m_rawPtrToBlock);
			uint* lv_temp = reinterpret_cast<uint*>(m_nextFreeMemoryBlock);
			*lv_temp = lv_index;
		}
		else {
			m_nextFreeMemoryBlock = reinterpret_cast<uchar*>(l_blockToDeallocate.m_rawPtrToBlock);
			uint* lv_temp = reinterpret_cast<uint*>(m_nextFreeMemoryBlock);
			*lv_temp = m_totalNumBlocks;
		}

		l_blockToDeallocate.m_rawPtrToBlock = nullptr;

		++m_totalNumFreeBlocks;
		--m_totalNumAllocatedBlocks;
		++m_sizeOfFreeBlocksList;
	}


	~MemoryPool()
	{
		 free((void*)m_startOfMemoryPool);
	}


#ifdef _DEBUG

	uint GetIndexOfNextPtr()
	{
		if (nullptr == m_nextFreeMemoryBlock) {
			return m_totalNumBlocks;
		}
		else {
			uint* lv_temp = reinterpret_cast<uint*>(m_nextFreeMemoryBlock);
			return *lv_temp;
		}
	}


	template<typename T>
	MemoryWrapper<T> AllocateNewBlockDebug()
	{
		auto lv_allocatedBlock = AllocateNewBlock<T>();
		std::cout << "Allocation: m_nextFreeMemoryBlock points to a block with index value " << GetIndexOfNextPtr() << std::endl;
		return lv_allocatedBlock;
	}



	template<typename T>
	void DeallocateBlockDebug(MemoryWrapper<T>& l_blockToDeallocate)
	{
		DeallocateBlock(l_blockToDeallocate);
		std::cout << "Deallocation: m_nextFreeMemoryBlock points to a block with index value " << GetIndexOfNextPtr() << std::endl;
	}


	void PrintFromStartToTheNextPtr()
	{
		std::cout << "------------New print from start--------" << std::endl;
		for (uchar* i = m_startOfMemoryPool; i < m_nextFreeMemoryBlock; i += m_sizeOfEachBlock) {
			uint* lv_temp = (uint*)(i);
			std::cout << *lv_temp << std::endl;
		}
	}

#endif

private:

	uchar* FindAddressNextFreeBlockFromIndex(const uint l_index);
	uint FindIndexFromAddress(const uchar* l_address);

	//This is used to make sure we do not free an already free MemoryWrapper instance
	bool IsAddressInFreeListBlocks(void* l_address);

	void CheckAddressRange(void* l_address);


	//void* EnlargeMemoryPool();

private:

	uint m_sizeOfEachBlock;
	uint m_totalNumBlocks;
	uint m_totalNumFreeBlocks;
	uint m_totalNumAllocatedBlocks;
	uint m_indexOfNextFreeBlock;
	uint m_sizeOfFreeBlocksList;

	uchar* m_startOfMemoryPool{};
	uchar* m_nextFreeMemoryBlock{};
	
};