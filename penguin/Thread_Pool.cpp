/*
* Copyright (c) 2025 Michael Mathers
*/
#include "Thread_Pool.h"

namespace Penguin
{
	Thread_Pool_Task::Thread_Pool_Task(void)
	{
	}


	Thread_Pool_Task::~Thread_Pool_Task(void)
	{
		this->interrupt();
	}


	int
	Thread_Pool_Task::interrupt(void)
	{
		return 0;
	}


	int
	Thread_Pool_Task::run(void)
	{
		return 0;
	}


	void
	Thread_Pool_Task::set_stop_token(const std::stop_token& stop_token)
	{
		this->stopToken_ = stop_token;
		/// TO-DO Establish a callback leveraging the interrupt function (if we can)
	}



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
		this->taskQueue_.push(task);
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
				std::stop_callback callback(stop_token, task.value()->interrupt());
				task.value()->set_stop_token(stop_token);
				task.value()->run();
				// Don't return here, allow the thread to pick up the next task from the queue
			}
		}
		
		return 0;
	}
}