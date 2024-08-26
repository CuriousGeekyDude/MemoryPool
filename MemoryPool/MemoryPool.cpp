

#include "MemoryPool.hpp"


MemoryPool::uchar* MemoryPool::FindAddressNextFreeBlockFromIndex(const uint l_index)
{
	if (m_totalNumBlocks <= l_index) {
		std::cerr << "Out of bounds error: Index exceeds total number of allocated blocks" << std::endl;
		exit(EXIT_FAILURE);
	}

	return m_startOfMemoryPool + (l_index*m_sizeOfEachBlock);
}


MemoryPool::uint MemoryPool::FindIndexFromAddress(const uchar* l_address)
{
	
	CheckAddressRange((void*)l_address);
	
	return static_cast<uint>((l_address - m_startOfMemoryPool) / m_sizeOfEachBlock);
	
}


void MemoryPool::CheckAddressRange(void* l_address)
{
	//This can happen when the pointer has already been deallocated so we dont want to exit, therefore we just return and do nothing
	if (nullptr == l_address) {
		return;
	}
	uchar* lv_lastElement = m_startOfMemoryPool + ((m_totalNumBlocks - 1) * m_sizeOfEachBlock);

	if ((uintptr_t)(l_address) < (uintptr_t)m_startOfMemoryPool || (uintptr_t)lv_lastElement < (uintptr_t)l_address) {
		std::cerr << "The pointer is out of the range of the pool." << std::endl;
		exit(EXIT_FAILURE);
	}

}



bool MemoryPool::IsAddressInFreeListBlocks(void* l_address)
{

	if (nullptr == m_nextFreeMemoryBlock) {
		return false;
	}

	uchar* lv_tempBlock = m_nextFreeMemoryBlock;

	for (uint i = 0; i < m_sizeOfFreeBlocksList; ++i) {

		uint* lv_tempuintBlock = reinterpret_cast<uint*>(lv_tempBlock);

		if (lv_tempBlock == reinterpret_cast<uchar*>(l_address)) {
			return true;
		}

		if (i < m_sizeOfFreeBlocksList-1) {
			lv_tempBlock = FindAddressNextFreeBlockFromIndex(*lv_tempuintBlock);
		}
	}

	return false;
}


//void* MemoryPool::EnlargeMemoryPool()
//{
//	size_t lv_sizeOfNewMemoryPool{ 2 * m_sizeOfEachBlock * m_totalNumBlocks };
//
//	auto lv_newStartMemoryPool = reinterpret_cast<uchar*>(realloc((void*)m_startOfMemoryPool, lv_sizeOfNewMemoryPool));
//
//	if (nullptr == lv_newStartMemoryPool) {
//		free(m_startOfMemoryPool);
//		throw std::runtime_error{ "realloc() failed." };
//	}
//	
//	m_startOfMemoryPool = lv_newStartMemoryPool;
//
//	
//	m_nextFreeMemoryBlock = m_startOfMemoryPool + m_sizeOfEachBlock*m_totalNumBlocks;
//	
//	m_totalNumFreeBlocks = m_totalNumBlocks;
//	m_totalNumBlocks = 2 * m_totalNumBlocks;
//
//	return m_nextFreeMemoryBlock;
//}
