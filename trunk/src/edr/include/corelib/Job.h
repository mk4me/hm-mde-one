/********************************************************************
	created:  2015/08/12	13:21:35
	filename: Job.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__JOB_H__
#define __HEADER_GUARD_CORELIB__JOB_H__

#include <string>
#include <corelib/Export.h>
#include <boost/thread/tss.hpp>
#include <utils/SmartPtr.h>

namespace core
{
	class IJobManager;

	class CORELIB_EXPORT JobBase
	{
		friend class IJobManager;

	public:

		struct Description
		{
			std::string owner;
			std::string name;
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

		friend struct JobGuard;

	private:		

		typedef boost::thread_specific_ptr<utils::shared_ptr<Description>> TLS;

	protected:

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
		JobBase& operator=(JobBase&& Other) NOEXCEPT
		{
			description = std::move(Other.description);
			return *this;
		}

		JobBase& operator=(const JobBase&) = delete;

		void swap(JobBase & Other) NOEXCEPT
		{
			std::swap(description, Other.description);
		}

	public:

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

	template<typename T>
	class Job : public JobBase, public InnerJobManager::InterruptibleJob<T>
	{
		friend class IJobManager;
	public:

		typedef InnerJobManager::InterruptibleJob<T> InnerJob;

	private:

		Job(InnerJob && Other, utils::shared_ptr<Description> description)
			: InnerJob(std::move(Other)), JobBase(description)
		{

		}

	public:

		Job() = default;

		Job(Job && Other) : JobBase(std::move(static_cast<JobBase&>(Other))),
			InnerJob(std::move(static_cast<InnerJob&>(Other)))
		{

		}

		Job(const Job & Other) = delete;

		virtual ~Job()
		{

		}

		//! \param Other W¹tek którego zasoby przejmujemy
		Job& operator=(Job&& Other) NOEXCEPT
		{
			this->JobBase::operator =(std::move(static_cast<JobBase&>(Other)));
			this->InnerJob::operator =(std::move(static_cast<InnerJob&>(Other)));
			return *this;
		}

		Job& operator=(const Job&) = delete;

		void swap(Job & Other) NOEXCEPT
		{
			std::swap<JobBase>(*this, Other);
			std::swap<InnerJob>(*this, Other);
		}
	};
}

#endif	// __HEADER_GUARD_CORELIB__JOB_H__