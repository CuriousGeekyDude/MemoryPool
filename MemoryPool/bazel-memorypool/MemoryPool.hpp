#pragma once


#include <stdexcept>
#include <iostream>

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
		MemoryWrapper(T* l_rawPtrToBlock, const unsigned int l_sizeOfEachBlock) : m_rawPtrToBlock(l_rawPtrToBlock),
			m_sizeOfEachBlock(l_sizeOfEachBlock){}

		MemoryWrapper(const MemoryWrapper& l_memoryWrapper) = delete;
		void operator=(const MemoryWrapper& l_memoryWrapper) = delete;

		MemoryWrapper(MemoryWrapper&& l_memoryWrapper) noexcept = default;

		void WriteToBlock(const T& m_srcData, const unsigned int l_sizeData)
		{
			if (m_sizeOfEachBlock < l_sizeData) {
				std::cerr << "Memory overflow: the size of data exceeds the size of allocated block." << std::endl;
				exit(EXIT_FAILURE);
			}

			else {
				memcpy((void*)m_rawPtrToBlock, (void*)&m_srcData, sizeof(T));
			}
		}

#ifdef _DEBUG

		unsigned int GetIndexOfBlock()
		{
			uint* lv_temp = reinterpret_cast<uint*>(m_rawPtrToBlock);
			return *lv_temp;
		}
#endif


	private:
		T* m_rawPtrToBlock{};
		unsigned int m_sizeOfEachBlock{};
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

		//Unlike malloc "new" never returns null.If it fails to allocate memory it throws std::bad_alloc exception.
		m_startOfContinuousMemory = new uchar[l_sizeOfEachBlock * l_totalNumBlocks];
		m_nextFreeMemoryBlock = m_startOfContinuousMemory;
	}

	MemoryPool(const MemoryPool&) = delete;
	void operator=(const MemoryPool&) = delete;

	template<typename T>
	MemoryWrapper<T> AllocateNewBlock()
	{
		void* lv_blockToAllocate{};

		if (0 != m_totalNumFreeBlocks) {

			if (1 < m_sizeOfFreeBlocksList) {
				--m_sizeOfFreeBlocksList;
				lv_blockToAllocate = reinterpret_cast<void*>(m_nextFreeMemoryBlock);
				if (1 != m_sizeOfFreeBlocksList) {
					m_nextFreeMemoryBlock = FindAddressNextFreeBlockFromIndex(*(reinterpret_cast<uint*>(m_nextFreeMemoryBlock)));
				}
				else {
					m_nextFreeMemoryBlock = nullptr;
				}
			}

			else {

				uint* lv_temp = reinterpret_cast<uint*>(m_nextFreeMemoryBlock);

				if (m_totalNumBlocks == *lv_temp) {
					lv_blockToAllocate = reinterpret_cast<void*>(m_nextFreeMemoryBlock);
					m_nextFreeMemoryBlock = nullptr;
					m_indexOfNextFreeBlock = 0;
				}
				else {
					++m_indexOfNextFreeBlock;
					*lv_temp = m_indexOfNextFreeBlock;
					lv_blockToAllocate = reinterpret_cast<void*>(m_nextFreeMemoryBlock);

					if (m_indexOfNextFreeBlock < m_totalNumBlocks)
						m_nextFreeMemoryBlock = FindAddressNextFreeBlockFromIndex(m_indexOfNextFreeBlock);
					else
						m_nextFreeMemoryBlock = nullptr;
				}

			}
			--m_totalNumFreeBlocks;
			++m_totalNumAllocatedBlocks;
		}

		return MemoryWrapper<T>{ reinterpret_cast<T*>(lv_blockToAllocate), m_sizeOfEachBlock};
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

		//l_blockToDeallocate.m_rawPtrToBlock->~T();
		l_blockToDeallocate.m_rawPtrToBlock = nullptr;

		++m_totalNumFreeBlocks;
		--m_totalNumAllocatedBlocks;
		++m_sizeOfFreeBlocksList;
	}


	~MemoryPool()
	{
		delete[] m_startOfContinuousMemory;
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

#endif

private:

	uchar* FindAddressNextFreeBlockFromIndex(const uint l_index);
	uint FindIndexFromAddress(const uchar* l_address);
	bool IsAddressInFreeListBlocks(void* l_address);
	void CheckAddressRange(void* l_address);

private:

	uint m_sizeOfEachBlock;
	uint m_totalNumBlocks;
	uint m_totalNumFreeBlocks;
	uint m_totalNumAllocatedBlocks;
	uint m_indexOfNextFreeBlock;
	uint m_sizeOfFreeBlocksList;

	uchar* m_startOfContinuousMemory{};
	uchar* m_nextFreeMemoryBlock{};
	
};