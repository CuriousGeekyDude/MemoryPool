


#include "MemoryPool.hpp"
#include <exception>
#include <malloc.h>
#include <iostream>
#include <string>

struct Test
{

	float m_firstNumber{};
	float m_secondNumber{};
};

int main()
{
	try
	{
		MemoryPool lv_pool{ sizeof(Test), 5};

		auto lv_test1 = lv_pool.AllocateNewBlock<Test>();
		auto lv_test2 = lv_pool.AllocateNewBlock<Test>();
		auto lv_test3 = lv_pool.AllocateNewBlock<Test>();
		auto lv_test4 = lv_pool.AllocateNewBlock<Test>();
		auto lv_test5 = lv_pool.AllocateNewBlock<Test>();
		

		/*lv_pool.DeallocateBlock((void**)&lv_test3);
		lv_pool.DeallocateBlock((void**)(&lv_test1));
		lv_pool.DeallocateBlock((void**)(&lv_test4));
		lv_pool.DeallocateBlock((void**)(&lv_test2));
		lv_pool.DeallocateBlock((void**)(&lv_test5));


		lv_test5 = (Test*)lv_pool.AllocateNewBlock();
		lv_test2 = (Test*)lv_pool.AllocateNewBlock();

		Test* lv_test = lv_test2;
		Test* lv_test0 = lv_test2;
		lv_pool.DeallocateBlock((void**)(&lv_test2));
		lv_pool.DeallocateBlock((void**)(&lv_test));
		lv_pool.DeallocateBlock((void**)(&lv_test0));*/

		
	}
	catch (const std::exception& l_error)
	{
		std::cerr << l_error.what() << std::endl;
	}
}