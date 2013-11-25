#ifndef HEADER_GUARD_UTILS__CLONEPOLICIES_H__
#define HEADER_GUARD_UTILS__CLONEPOLICIES_H__

#include <exception>
#include <osg/CopyOp>
#include <osg/Object>
//Plik zawiera podstawowe zasady kopiowania obiektów domenowych

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

    //! Klonowanie odbywa się za pomoca konstruktora kopiującego
    struct ClonePolicyCopyConstructor
    {
        template <typename T>
        T* clone(const T* s) const {
            return new T(*s);
        }
    };

    //! Klonowanie odbywa się za pomoca metody clone()
    struct ClonePolicyVirtualCloneMethod
    {
        template <typename T>
        T* clone(T* s) const {
            auto copy = s->clone();
            T* result = dynamic_cast<T*>(copy);
            UTILS_ASSERT(result);
            return result;
        }
    };

    //! Klonowanie odbywa się za pomoca metody clone()
    struct ClonePolicyOsgCloneMethod
    {
        template <typename T>
        T* clone(T* s) const {
            auto copy = osg::clone(s, osg::CopyOp::DEEP_COPY_ALL);
            T* result = dynamic_cast<T*>(copy);
            UTILS_ASSERT(result);
            return result;
        }
    };

    //! Powinno podawać się ta zasade, jeśli klonowanie z jakiegos powodu jest zabronione
    struct ClonePolicyForbidden
    {
        template <typename T>
        T* clone(const T* s) const {
            //UTILS_STATIC_ASSERT(false, "Cloning forbidden!");
            throw std::runtime_error("Cloning forbidden!");
        }
    };



    typedef ClonePolicyForbidden ClonePolicyNotImplemented;

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif    // HEADER_GUARD_UTILS__CLONEPOLICIES_H__