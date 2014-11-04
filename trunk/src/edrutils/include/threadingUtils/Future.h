/********************************************************************
	created:  2014/10/28	23:23:04
	filename: Future.h
	author:	  Mateusz Janiak

	purpose:  Poprawka dla implementacji pod VS2013 dla packaged_task<void(...)>
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__FUTURE_H__
#define __HEADER_GUARD_THREADINGUTILS__FUTURE_H__

#include <threadingUtils/Macros.h>
#include <utils/SmartPtr.h>
#include <future>

#ifdef FIX_VOID_RET_PACKAGED_TASK

namespace std
{
	template<class... _ArgTypes>
	class packaged_task < void(_ArgTypes...) >
	{
		promise<void> _my_promise;
		utils::shared_ptr<function<void(_ArgTypes...)>> _my_func;

	public:
		packaged_task() {}

		template<class _Fty2>
		explicit packaged_task(_Fty2&& _Fnarg)
			: _my_func(utils::make_shared<function<void(_ArgTypes...)>>(_Fnarg)) {
		}

		packaged_task(packaged_task&& _Other)
			: _my_promise(move(_Other._my_promise)),
			_my_func(move(_Other._my_func)) {
		}

		packaged_task& operator=(packaged_task&& _Other) {
			_my_promise = move(_Other._my_promise);
			_my_func = move(_Other._my_func);
			return (*this);
		}

		packaged_task(const packaged_task&) = delete;
		packaged_task& operator=(const packaged_task&) = delete;

		~packaged_task() {
		}

		void swap(packaged_task& _Other) {
			swap(_my_promise, _Other._my_promise);
			swap(_my_func, _Other._my_func);
		}

		explicit operator bool() const {
			return _my_func != nullptr && (*_my_func) != false;
		}

		bool valid() const {
			return _my_func != nullptr && (*_my_func) != false;
		}

		future<void> get_future() {
			return _my_promise.get_future();
		}

		void operator()(_ArgTypes... _Args) {
			(*_my_func)(forward<_ArgTypes>(_Args)...);
			_my_promise.set_value();
		}

		void reset() {
			_my_promise.swap(promise<void>());
			_my_func.reset();
		}
	};
}

#endif

#endif	// __HEADER_GUARD_THREADINGUTILS__FUTURE_H__