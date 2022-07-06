#include <thread>
#include "elekicore/array.hpp"
#include "elekicore/set.hpp"
#include "elekicore/map.hpp"
#include "elekicore/tasks.hpp"

using namespace ElekiEngine;

//
// ThreadPool
// -----

class ThreadPool
{
	struct Node
	{
		void *id;
		Func<void()> run;
		Node *prev;
		Node *next;
	};

	std::condition_variable mCondition;

	Mutex mIsRunningLockFlag;
	bool mIsRunning;

	Mutex mThreadQueueAndMapLockFlag;
	Map<void *, Node *> mThreadMap;
	Node *mThreadQueueFirst;
	Node *mThreadQueueLast;

	Mutex mFinishedSetLockFlag;
	Set<void *> mFinishedSet;

	size_t mThreadListSize;
	std::thread *mThreadList;

	void setIsRunning(bool isRunning)
	{
		Lock lock(mIsRunningLockFlag);
		mIsRunning = isRunning;
	}

	bool getIsRunning()
	{
		Lock lock(mIsRunningLockFlag);
		return mIsRunning;
	}

	Node *newNode(void *id, const Func<void()> &func)
	{
		auto node = new(Memory::allocate(sizeof(Node))) Node();
		node->id = id;
		node->next = nullptr;
		node->prev = nullptr;
		node->run = func;

		return node;
	}

	void deleteNode(Node *node)
	{
		Memory::deallocate(node);
	}

	void enqueue(Node *node)
	{
		if(mThreadQueueFirst)
		{
			mThreadQueueFirst->next = node;
		}
		else
		{
			mThreadQueueLast = node;
		}
		node->prev = mThreadQueueFirst;
		mThreadQueueFirst = node;

		mThreadMap.add(node->id, node);
	}

	Node *dequeue()
	{
		auto node = mThreadQueueLast;
		mThreadQueueLast = node->next;
		if(mThreadQueueLast)
		{
			mThreadQueueLast->prev = nullptr;
		}
		else
		{
			mThreadQueueFirst = nullptr;
		}

		mThreadMap.remove(node->id);

		return node;
	}

	void work()
	{

		while(true)
		{
			bool isRunning = getIsRunning();

			Node *node = nullptr;

			{
				Lock lock(mThreadQueueAndMapLockFlag);
				mCondition.wait(lock, [&]
				{
					return mThreadQueueLast || !mIsRunning;
				});

				if(mThreadQueueLast)
				{
					node = dequeue();
				}
				else if(!mIsRunning)
				{
					return;
				}
			}

			if(node)
			{
				node->run();

				{
					Lock lock(mFinishedSetLockFlag);
					mFinishedSet.add(node->id);
				}

				deleteNode(node);
			}
		}
	}

	Node *remove(void *id)
	{
		Lock lock(mThreadQueueAndMapLockFlag);
		if(mThreadMap.contains(id))
		{
			auto node = mThreadMap[id];
			mThreadMap.remove(id);

			if(node->prev) node->prev->next = node->next;
			else mThreadQueueFirst = node->next;
			if(node->next) node->next->prev = node->prev;
			else mThreadQueueLast = node->prev;

			return node;
		}
		else
		{
			return nullptr;
		}
	}

	void wait(void *id)
	{
		while(true)
		{
			Lock lock(mFinishedSetLockFlag);
			if(mFinishedSet.contains(id))
			{
				mFinishedSet.remove(id);
				return;
			}
		}
	}

	void newPool()
	{
		setIsRunning(true);

		mThreadListSize = std::thread::hardware_concurrency();
		mThreadList = (std::thread*)malloc(sizeof(std::thread) * mThreadListSize);

		for(size_t i = 0; i < mThreadListSize; i++)
		{
			new(&mThreadList[i]) std::thread(bind(&ThreadPool::work, this));
		}
	}

	void deletePool()
	{
		setIsRunning(false);

		for(size_t i = 0; i < mThreadListSize; i++)
		{
			mThreadList[i].join();
		}

		free(mThreadList);
	}

public:

	ThreadPool()
	{
		newPool();
	}

	~ThreadPool()
	{
		deletePool();
	}

	bool add(void* id, const Func<void()> &func)
	{
		if(getIsRunning())
		{
			auto node = newNode(id, func);

			Lock lock(mThreadQueueAndMapLockFlag);
			enqueue(node);
			return true;
		}
		else
		{
			return false;
		}
	}

	// 強制
	void run(void *id)
	{
		auto node = remove(id);
		if(node)
		{
			node->run();
			deleteNode(node);
		}
		else
		{
			wait(id);
		}
	}

	bool finished(void *id)
	{
		Lock lock(mFinishedSetLockFlag);
		return mFinishedSet.contains(id);
	}
} 
*gThreadPool;

std::once_flag gInitThreadsOnceFlag;
void initThreadPool()
{
	gThreadPool = new(malloc(sizeof(ThreadPool))) ThreadPool();
}

struct FinThreadPool
{
	~FinThreadPool()
	{
		if(gThreadPool)
		{
			gThreadPool->~ThreadPool();
			free(gThreadPool);
		}
	}
} gFinThreadPool;

//
// Thread
// -----

// スレッドを初期化します
void initThread(Thread *thread, EThreadMode mode)
{
	
}

// 初期化します
void ElekiEngine::Thread::init()
{
	std::call_once(gInitThreadsOnceFlag, initThreadPool);
	switch(mMode)
	{
		case ElekiEngine::EThreadMode::THREAD_POOL:
		gThreadPool->add(this, bind(&Thread::run, this));
		break;

		case ElekiEngine::EThreadMode::INDEPENDENCE:
		mIndependenceThread = new(Memory::allocate(sizeof(std::thread))) std::thread(bind(&Thread::run, this));
		break;

		default:
		break;
	}
}

// 終了化します
void ElekiEngine::Thread::fin()
{
	switch(mMode)
	{
		case ElekiEngine::EThreadMode::THREAD_POOL:
		gThreadPool->run(this);
		break;

		case ElekiEngine::EThreadMode::INDEPENDENCE:
		mIndependenceThread->join();
		mIndependenceThread->~thread();
		Memory::deallocate(mIndependenceThread);
		break;

		default:
		break;
	}
}

// コンストラクタです
ElekiEngine::Thread::Thread(EThreadMode mode)
	: mMode(mode)
{
	init();
}

// デストラクタです
ElekiEngine::Thread::~Thread()
{
	join();
}

// 並列処理の終了を待ち、スレッドを結合します
void ElekiEngine::Thread::join()
{
	fin();
}

// デストラクタです
bool ElekiEngine::Thread::ended()
{
	return gThreadPool->finished(this); // これ　＋　!mIsRunning　add run で　waitを回避する方法
}