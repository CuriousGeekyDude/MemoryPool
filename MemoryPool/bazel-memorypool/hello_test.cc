#include <gtest/gtest.h>
#include "MemoryPool.hpp"




struct Test
{

	float m_firstNumber{};
	float m_secondNumber{};
};


TEST(MemoryPoolAllocator, UnitTest) {
	
	MemoryPool lv_pool{ sizeof(Test), 10 };

	auto lv_block1 = lv_pool.AllocateNewBlock<Test>();
	auto lv_block2 = lv_pool.AllocateNewBlock<Test>();
	auto lv_block3 = lv_pool.AllocateNewBlock<Test>();
	auto lv_block4 = lv_pool.AllocateNewBlock<Test>();
	auto lv_block5 = lv_pool.AllocateNewBlock<Test>();
	auto lv_block6 = lv_pool.AllocateNewBlock<Test>();

	lv_pool.DeallocateBlock<Test>(lv_block3);
	ASSERT_EQ(lv_pool.GetIndexOfNextPtr(), 5);
	lv_pool.DeallocateBlock<Test>(lv_block5);
	ASSERT_EQ(lv_pool.GetIndexOfNextPtr(), 2);
	lv_pool.DeallocateBlock<Test>(lv_block1);
	ASSERT_EQ(lv_pool.GetIndexOfNextPtr(), 4);

	lv_pool.AllocateNewBlock<Test>();
	ASSERT_EQ(lv_pool.GetIndexOfNextPtr(), 2);
	lv_pool.AllocateNewBlock<Test>();
	ASSERT_EQ(lv_pool.GetIndexOfNextPtr(), 5);
	lv_pool.AllocateNewBlock<Test>();
	ASSERT_EQ(lv_pool.GetIndexOfNextPtr(), 6);

}