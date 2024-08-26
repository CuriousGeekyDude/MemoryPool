


#include "MemoryPool.hpp"
#include <exception>
#include <malloc.h>
#include <iostream>
#include <string>


#ifdef _DEBUG
#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

#endif



#ifdef _DEBUG

//Struct used to test the memory pool with
struct Test
{
	Test(const float l_firstNumber, const float l_secondNumber, const float 
	l_third, const float l_fourth) : m_firstNumber(l_firstNumber),
		m_secondNumber(l_secondNumber), m_third(l_third), m_fourth(l_fourth) { }
	Test() = default;

	float m_firstNumber{};
	float m_secondNumber{};
	float m_third{};
	float m_fourth{};
};

#endif



int main()
{
	try
	{


#ifdef _DEBUG

		
		MemoryPool lv_pool1{sizeof(Test), 256};



		ankerl::nanobench::Bench().minEpochIterations(2179).run("Memory pool benchmark for 256 allocations",
			[&] {
				
				for (int i = 0; i < 256; ++i) {
					lv_pool1.AllocateNewBlock<Test>();
				}
			});


		ankerl::nanobench::Bench().minEpochIterations(21709).run("malloc benchmark for 256 allocations",
			[&] {
				for (int i = 0; i < 256; ++i) {
					malloc(sizeof(Test));
				}
			});

#endif
		
	}
	catch (const std::exception& l_error)
	{
		std::cerr << l_error.what() << std::endl;
	}
}