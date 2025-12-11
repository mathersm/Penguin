/*
* Copyright (c) 2025 Michael Mathers
*/
#include "Thread_Pool.h"

namespace Penguin
{
	Thread_Pool::Thread_Pool(uint32_t initial_thread_count)
		: shutdown_(false)
		, threads_(initial_thread_count)
	{
		for (auto& thread : this->threads_)
		{
			thread = std::jthread(Thread_Pool::thread_function, this);
		}
	}


	Thread_Pool::~Thread_Pool(void)
	{
		this->shutdown_ = true;

		for (auto& thread : this->threads_)
		{
			thread.request_stop();
			thread.join();
		}
	}


	int
	Thread_Pool::execute(const std::shared_ptr<Penguin::Thread_Pool_Task>& task)
	{
		this->taskQueue_.push(task);
		return 0;
	}


	int
	Thread_Pool::execute(std::shared_ptr<Penguin::Thread_Pool_Task>&& task)
	{
		this->taskQueue_.push(std::move(task));
		return 0;
	}


	int
	Thread_Pool::thread_function(std::stop_token stop_token, Thread_Pool* pool_ptr)
	{
		while (!pool_ptr->shutdown_)
		{
			// Don't block forever, poll the queue allowing space for interrupts and stop requests
			std::optional<std::shared_ptr<Penguin::Thread_Pool_Task>> task = pool_ptr->taskQueue_.try_pop_for(Thread_Pool::RETRIEVAL_TIMEOUT);
			if (task.has_value())
			{
				/// TO-DO Add in a mechanism to allow a callback on interruption
				// std::stop_callback callback(stop_token, [task] {task.value().interrupt(); });
				
				task.value()->run();
				// Don't return here, allow the thread to pick up the next task from the queue
			}
		}
		
		return 0;
	}
}