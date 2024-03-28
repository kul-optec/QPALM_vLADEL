#pragma once

#include "demangled-typename.hpp"
#include <optional>
#include <set>
#include <stdexcept>

namespace qpalm {

template <class T>
const T *get_identity(const T &t) {
    return std::addressof(t);
}
template <class T>
void get_identity(const T *) = delete;

template <class T>
class ThreadChecker {
    using set_t      = std::set<decltype(get_identity(std::declval<T>()))>;
    using iterator_t = typename set_t::iterator;
    static set_t set;
    std::optional<iterator_t> iterator;

  public:
    ThreadChecker(const T &t) {
        auto [iter, inserted] = set.insert(get_identity(t));
        if (!inserted) {
            std::string name = "instance of type " + demangled_typename(typeid(T));
            throw std::runtime_error("Same " + name +
                                     " used in multiple threads (consider making a copy or "
                                     "creating a separate instance for each thread)");
        }
        iterator = iter;
    }
    ~ThreadChecker() {
        if (iterator)
            set.erase(*iterator);
    }
    ThreadChecker(const ThreadChecker &)            = delete;
    ThreadChecker &operator=(const ThreadChecker &) = delete;
    ThreadChecker(ThreadChecker &&o) noexcept { std::swap(this->iterator, o.iterator); }
    ThreadChecker &operator=(ThreadChecker &&o) noexcept {
        this->iterator = std::move(o.iterator);
        o.iterator.reset();
        return *this;
    }
};

template <class T>
typename ThreadChecker<T>::set_t ThreadChecker<T>::set;

} // namespace qpalm
