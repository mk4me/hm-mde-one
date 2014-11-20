/********************************************************************
	created:  2014/10/26	16:59:40
	filename: JobManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__JOBMANAGER_H__
#define __HEADER_GUARD_CORELIB__JOBMANAGER_H__

#include <string>
#include <utils/SmartPtr.h>
#include <boost/thread/tss.hpp>
#include <threadingUtils/InterruptibleJobManager.h>
#include <threadingUtils/InterruptibleWorkManager.h>
#include <threadingUtils/WorkQueuePolicy.h>
#include <corelib/ThreadPool.h>
#include <corelib/Export.h>
#include <corelib/ILog.h>

namespace core
{
	class CORELIB_EXPORT JobManager
	{

	private:

		struct ErrorLogCallPolicy
		{
			//! \tparam F Typ metody jak¹ wo³amy
			//! \tparam Args Typy argumentów z jakimi wo³amy metodê
			//! \param f Wo³ana metoda
			//! \param arguments Argumenty metody
			template<class F, class... Args>
			static typename std::result_of<F(Args...)>::type call(F&& f, Args&&... arguments)
			{
				try{
					return f(arguments...);
				}
				catch (std::exception & e){
					JobManager::logError(UTILS_FORMAT_STRING(" failed with error: " << e.what()));
					throw;
				}
				catch (...){
					JobManager::logError(" failed with UNKNOWN error");
					throw;
				}
			}
		};

	public:

		struct JobGuard;

		struct Description
		{
			std::string owner;
			std::string name;
		};

	public:
		typedef threadingUtils::InterruptibleWorkManager<threadingUtils::StealingMultipleWorkQueuePolicy, ThreadPool::Thread, ErrorLogCallPolicy> InnerWorkManager;
		typedef threadingUtils::InterruptibleJobManager<InnerWorkManager> InnerJobManager;

		class CORELIB_EXPORT JobBase
		{			
			friend struct JobGuard;

		private:

			typedef boost::thread_specific_ptr<utils::shared_ptr<Description>> TLS;

		public:

			JobBase() = default;

			JobBase(const JobBase & Other) = delete;

			JobBase(utils::shared_ptr<Description> description)
				: description(description)
			{

			}

			JobBase(JobBase && Other) : description(std::move(Other.description))
			{

			}

			virtual ~JobBase()
			{

			}

			//! \param Other W¹tek którego zasoby przejmujemy
			JobBase& operator=(JobBase&& Other) noexcept
			{
				description = std::move(Other.description);
				return *this;
			}

			JobBase& operator=(const JobBase&) = delete;

			void swap(JobBase & Other) noexcept
			{
				std::swap(description, Other.description);
			}

			const std::string owner() const
			{
				return description != nullptr ? description->owner : "No explicit owner";
			}

			const std::string name() const
			{
				return description != nullptr ? description->name : "No explicit name";
			}

			static const std::string currentOwner()
			{
				return (*tlsJD)->owner;
			}

			static const std::string currentName()
			{
				return (*tlsJD)->name;
			}

		private:

			utils::shared_ptr<Description> description;
			static TLS tlsJD;
		};

		struct JobGuard
		{
			JobGuard(utils::shared_ptr<Description> description)
			{
				if (JobBase::tlsJD.get() == nullptr){
					JobBase::tlsJD.reset(new utils::shared_ptr<Description>(description));
				}
				else{
					(*JobBase::tlsJD) = description;
				}
			}

			~JobGuard()
			{
				(*(*JobBase::tlsJD)) = Description({ "No explicit owner", "No explicit name" });
			}
		};

		template<typename T, typename InterruptibleWorkManager>
		class Job : public JobBase, public threadingUtils::InterruptibleJob<T, InterruptibleWorkManager>
		{
		public:

			Job() = default;

			Job(const Job & Other) = delete;

			Job(threadingUtils::InterruptibleJob<T, InterruptibleWorkManager> && Other, utils::shared_ptr<Description> description)
				: threadingUtils::InterruptibleJob<T, InterruptibleWorkManager>(std::move(Other)), JobBase(description)
			{

			}

			Job(Job && Other) : JobBase(std::move(static_cast<JobBase&>(Other))),
					threadingUtils::InterruptibleJob<T, InterruptibleWorkManager>(std::move(static_cast<threadingUtils::InterruptibleJob<T, InterruptibleWorkManager>&>(Other)))
			{

			}

			virtual ~Job()
			{

			}

			//! \param Other W¹tek którego zasoby przejmujemy
			Job& operator=(Job&& Other) noexcept
			{
				JobBase::operator =(std::move(static_cast<JobBase&>(Other)));
				threadingUtils::InterruptibleJob<T, InterruptibleWorkManager>::operator =(std::move(static_cast<threadingUtils::InterruptibleJob<T, InterruptibleWorkManager>&>(Other)));
				return *this;
			}

			Job& operator=(const Job&) = delete;

			void swap(Job & Other) noexcept
			{
				std::swap<JobBase>(*this, Other);
				std::swap<threadingUtils::InterruptibleJob<T, InterruptibleWorkManager>>(*this, Other);
			}
		};

	public:

		//! Domyœlny konstruktor
		JobManager(InnerJobManager * jm);
		//! Desturktor
		~JobManager() = default;

		template<typename F, class ...Args>
		Job<typename std::result_of<F(Args...)>::type, InnerWorkManager> create(const std::string & owner, const std::string & name, F&& f, Args&& ...arguments)
		{
			typedef typename std::result_of<F(Args...)>::type result_type;
			auto description = utils::make_shared<Description>(Description{ owner, name });

			std::function<result_type()> ff = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);

			auto innerJob = threadingUtils::create(
				[=]() -> result_type
			{
				try{
					JobGuard guard(description);
					return (result_type)ff();
				}
				catch (std::exception & e){
					JobManager::logError(UTILS_FORMAT_STRING(" failed with error: " << e.what()));
					throw;
				}
				catch (...){
					JobManager::logError(" failed with UNKNOWN error");
					throw;
				}
			});

			return Job<result_type, InnerWorkManager>(std::move(innerJob), description);
		}

		template<typename JobType>
		void waitForOtherJob(JobType & job)
		{
			jm->waitForOtherJob(job);
		}

		//! \param log Logger którego u¿ywamy do logowania
		static void setLog(LogPtr log);

	private:

		static void logError(const std::string & message);

	private:

		//! Wewnêtrzny tp
		InnerJobManager * jm;

		//! Logger
		static LogPtr log_;
	};
}

#endif	// __HEADER_GUARD_CORELIB__JOBMANAGER_H__
