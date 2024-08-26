

#include "MemoryPool.hpp"


//void MemoryPool::MemoryWrapper::WriteToBlock(void* m_srcData, const unsigned int l_sizeData)
//{
//	if (m_sizeOfEachBlock < l_sizeData) {
//		throw std::runtime_error{ "Memory overflow: the size of data exceeds the size of allocated block." };
//	}
//
//	else {
//		memcpy(m_rawPtrToBlock, m_srcData, (size_t)l_sizeData);
//	}
//}




MemoryPool::uchar* MemoryPool::FindAddressNextFreeBlockFromIndex(const uint l_index)
{
	if (m_totalNumBlocks <= l_index) {
		std::cerr << "Out of bounds error: Index exceeds total number of allocated blocks" << std::endl;
		exit(EXIT_FAILURE);
	}

	return m_startOfContinuousMemory + (l_index*m_sizeOfEachBlock);
}


MemoryPool::uint MemoryPool::FindIndexFromAddress(const uchar* l_address)
{
	
	CheckAddressRange((void*)l_address);
	
	return static_cast<uint>((l_address - m_startOfContinuousMemory) / m_sizeOfEachBlock);
	
}


void MemoryPool::CheckAddressRange(void* l_address)
{
	//This happens when the pointer has already been deallocated so we dont want to exit, therefore we just return and do nothing
	if (nullptr == l_address) {
		return;
	}
	uchar* lv_lastElement = m_startOfContinuousMemory + ((m_totalNumBlocks - 1) * m_sizeOfEachBlock);

	if ((uintptr_t)(l_address) < (uintptr_t)m_startOfContinuousMemory || (uintptr_t)lv_lastElement < (uintptr_t)l_address) {
		std::cerr << "The pointer is out of the range of the pool." << std::endl;
		exit(EXIT_FAILURE);
	}

}


//MemoryPool::MemoryWrapper MemoryPool::AllocateNewBlock()
//{
//
//	void* lv_blockToAllocate{};
//
//	if (0 != m_totalNumFreeBlocks) {
//
//		if (1 < m_sizeOfFreeBlocksList) {
//			--m_sizeOfFreeBlocksList;
//			lv_blockToAllocate = reinterpret_cast<void*>(m_nextFreeMemoryBlock);
//			if (1 != m_sizeOfFreeBlocksList) {
//				m_nextFreeMemoryBlock = FindAddressNextFreeBlockFromIndex(*(reinterpret_cast<uint*>(m_nextFreeMemoryBlock)));
//			}
//			else {
//				m_nextFreeMemoryBlock = nullptr;
//			}
//		}
//
//		else {
//			
//			uint* lv_temp = reinterpret_cast<uint*>(m_nextFreeMemoryBlock);
//
//			if (m_totalNumBlocks == *lv_temp) {
//				lv_blockToAllocate = reinterpret_cast<void*>(m_nextFreeMemoryBlock);
//				m_nextFreeMemoryBlock = nullptr;
//				m_indexOfNextFreeBlock = 0;
//			}
//			else {
//				++m_indexOfNextFreeBlock;
//				*lv_temp = m_indexOfNextFreeBlock;
//				lv_blockToAllocate = reinterpret_cast<void*>(m_nextFreeMemoryBlock);
//
//				if (m_indexOfNextFreeBlock < m_totalNumBlocks)
//					m_nextFreeMemoryBlock = FindAddressNextFreeBlockFromIndex(m_indexOfNextFreeBlock);
//				else
//					m_nextFreeMemoryBlock = nullptr;
//			}
//			
//		}
//		--m_totalNumFreeBlocks;
//		++m_totalNumAllocatedBlocks;
//	}
//
//	return MemoryWrapper{ lv_blockToAllocate, m_sizeOfEachBlock };
//}


//void MemoryPool::DeallocateBlock(MemoryWrapper& l_blockToDeallocate)
//{
//
//	CheckAddressRange(l_blockToDeallocate.m_rawPtrToBlock);
//
//	if (nullptr == l_blockToDeallocate.m_rawPtrToBlock || true == IsAddressInFreeListBlocks(l_blockToDeallocate.m_rawPtrToBlock)) {
//		return;
//	}
//
//	uint lv_index{};
//	if (nullptr != m_nextFreeMemoryBlock) {
//		lv_index = FindIndexFromAddress(m_nextFreeMemoryBlock);
//		m_nextFreeMemoryBlock = reinterpret_cast<uchar*>(l_blockToDeallocate.m_rawPtrToBlock);
//		uint* lv_temp = reinterpret_cast<uint*>(m_nextFreeMemoryBlock);
//		*lv_temp = lv_index;
//	}
//	else {
//		m_nextFreeMemoryBlock = reinterpret_cast<uchar*>(l_blockToDeallocate.m_rawPtrToBlock);
//		uint* lv_temp = reinterpret_cast<uint*>(m_nextFreeMemoryBlock);
//		*lv_temp = m_totalNumBlocks;
//	}
//
//	l_blockToDeallocate.m_rawPtrToBlock = nullptr;
//
//	++m_totalNumFreeBlocks;
//	--m_totalNumAllocatedBlocks;
//	++m_sizeOfFreeBlocksList;
//	
//}



bool MemoryPool::IsAddressInFreeListBlocks(void* l_address)
{

	if (nullptr == m_nextFreeMemoryBlock) {
		return false;
	}

	uchar* lv_tempBlock = m_nextFreeMemoryBlock;

	for (uint i = 0; i < m_sizeOfFreeBlocksList; ++i) {
		if (lv_tempBlock == reinterpret_cast<uchar*>(l_address)) {
			return true;
		}
		if (m_totalNumBlocks == *lv_tempBlock) {
			return false;
		}
		lv_tempBlock = FindAddressNextFreeBlockFromIndex(*lv_tempBlock);
	}

	return false;
}
