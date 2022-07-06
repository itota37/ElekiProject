/// @file tasks.hpp
/// @version 1.22.6
/// @copyright © 2022 Taichi Ito
/// 並行処理機能を提供します

#ifndef ELEKICORE_TASKS_HPP
#define ELEKICORE_TASKS_HPP

#include <mutex>
#include <type_traits>
#include "preprocess.hpp"
#include "functional.hpp"
#include "pointer.hpp"

/// ELEKi ENGINE
namespace ElekiEngine
{

	/// 排他ロックフラグ型エイリアスです
	using Mutex = std::mutex;

	/// 排他ロックガード型エイリアスです
	using Lock = std::unique_lock<Mutex>;

	/// スレッドモードの列挙です
	enum class EThreadMode
	{

		THREAD_POOL,  ///< スレッドプールを使い並列に実行します
		INDEPENDENCE, ///< 独立スレッドを立てて並列に実行します

	};

	/// 並列処理をコントロールするタスククラスです
	template<class R>class Task;

	/// 並列処理で実行します
	template<class R>
	UR<Task<R>> parallel(const Func<R()> &func, EThreadMode mode);

	/// スレッドクラスです
	class ELEKICORE_EXPORT Thread
	{
		EThreadMode mMode;                // スレッドモード
		std::thread *mIndependenceThread; // 独立スレッドモード用スレッド

		// 初期化します
		void init();
		// 終了化します
		void fin();

	public:

		/// コンストラクタです
		Thread(EThreadMode mode);

		/// デストラクタです
		virtual ~Thread();

		/// 並列に実行する処理です
		virtual void run() = 0;

		/// 並列処理の終了を待ち、スレッドを結合します
		void join();

		/// 並列処理が終了しているか判定します
		bool ended();
	};

	/// 並列処理をコントロールするタスククラスです
	template<class R>
	class Task: protected Thread
	{
		template<class R> friend UR<Task<R>> parallel(const Func<R()> &func, EThreadMode mode);
		
		std::remove_reference_t<R> mResult; // 戻り値
		Func<R()> mFunc;                    // 実行する関数

		// コンストラクタです
		Task(const Func<R()> &func, EThreadMode mode)
			: Thread(mode)
			, mFunc(func)
		{}

		// 並列に実行する処理です
		void run() override
		{
			mResult = mFunc();
		}

	public:

		Task(const Task<R> &) = delete;
		Task(Task<R> &&) noexcept = delete;
		Task<R> &operator=(const Task<R> &) = delete;
		Task<R> &operator=(Task<R> &&) noexcept = delete;

		/// デストラクタです
		~Task() override
		{}

		/// 並列処理の終了を待ち、合流します
		R marge()
		{
			join();
			return mResult;
		}

		/// 並列処理が終了しているか判定します
		bool finished()
		{
			return ended();
		}
	};

	/// 並列処理をコントロールするタスククラスです
	template<>
	class Task <void>: protected Thread
	{
		template<class R> friend UR<Task<R>> parallel(const Func<R()> &func, EThreadMode mode);

		Func<void()> mFunc; // 実行する関数

		// コンストラクタです
		Task(const Func<void()> &func, EThreadMode mode)
			: Thread(mode)
			, mFunc(func)
		{}

		// 並列に実行する処理です
		void run() override
		{
			mFunc();
		}

	public:

		Task(const Task<void> &) = delete;
		Task(Task<void> &&) noexcept = delete;
		Task<void> &operator=(const Task<void> &) = delete;
		Task<void> &operator=(Task<void> &&) noexcept = delete;

		/// デストラクタです
		~Task() override
		{
			marge();
		}

		/// 並列処理の終了を待ち、合流します
		void marge()
		{
			join();
		}

		/// 並列処理が終了しているか判定します
		bool finished()
		{
			return ended();
		}
	};

	/// 並列処理で実行します
	template<class R>
	UR<Task<R>> parallel(const Func<R()> &func, EThreadMode mode = EThreadMode::THREAD_POOL)
	{
		auto ptr = new(Memory::allocate(sizeof(Task<R>))) Task<R>(func);
		return UR<Task<R>>(ptr, Memory::deleter());
	}

}

#endif // !ELEKICORE_TASKS_HPP