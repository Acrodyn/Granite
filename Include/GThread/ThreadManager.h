#pragma once

#include <thread>
#include <vector>

class ThreadManager
{
public:
	static void Init();

	template<class T>
	static void ThreadedVectorProcessing(const std::vector<T>& data, void (*functionPtr)(), int numberOfThreads = 4)
	{
		std::thread threads[numberOfThreads];

		for (int i = 0; i < numberOfThreads; ++i)
		{
			threads[i] = std::thread(*functionPtr, data);
		}
		
		for (int i = 0; i < numberOfThreads; ++i)
		{
			threads[i].join();
		}
	}
};