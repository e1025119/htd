/* 
 * File:   ConstIterator.hpp
 * 
 * Author: ABSEHER Michael (abseher@dbai.tuwien.ac.at)
 * 
 * Copyright 2015-2016, Michael Abseher
 *    E-Mail: <abseher@dbai.tuwien.ac.at>
 * 
 * This file is part of htd.
 * 
 * htd is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free 
 * Software Foundation, either version 3 of the License, or (at your 
 * option) any later version.
 * 
 * htd is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public 
 * License for more details.

 * You should have received a copy of the GNU General Public License
 * along with htd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HTD_HTD_CONSTITERATOR_HPP
#define HTD_HTD_CONSTITERATOR_HPP

#include <htd/Globals.hpp>
#include <htd/ConstIteratorBase.hpp>
#include <htd/ConstIteratorWrapper.hpp>
#include <htd/Iterator.hpp>

#include <memory>

namespace htd
{
    template <typename T>
    class ConstIterator : public virtual htd::ConstIteratorBase<T>
    {
        public:
            typedef std::ptrdiff_t difference_type;
            typedef T value_type;
            typedef const T & reference;
            typedef const T * pointer;
            typedef const T & const_reference;
            typedef const T * const_pointer;
            typedef std::forward_iterator_tag iterator_category;

            ConstIterator(void) : baseIterator_(nullptr)
            {

            }

            template <class Iter>
            ConstIterator(Iter iterator) : baseIterator_(new htd::ConstIteratorWrapper<Iter, T>(iterator))
            {

            }

            ConstIterator<T>(const ConstIterator<T> & original) : baseIterator_(nullptr)
            {
                if (original.baseIterator_ != nullptr)
                {
                    baseIterator_ = original.baseIterator_->clone();
                }
            }

            virtual ~ConstIterator()
            {
                if (baseIterator_ != nullptr)
                {
                    delete baseIterator_;

                    baseIterator_ = nullptr;
                }
            }

            ConstIterator & operator=(htd::Iterator<T> & other)
            {
                if (baseIterator_ != nullptr)
                {
                    delete baseIterator_;

                    baseIterator_ = nullptr;
                }

                if (other.baseIterator_ != nullptr)
                {
                    baseIterator_ = other;
                }

                return *this;
            }

            ConstIterator & operator=(ConstIterator<T> & other)
            {
                if (baseIterator_ != nullptr)
                {
                    delete baseIterator_;

                    baseIterator_ = nullptr;
                }

                if (other.baseIterator_ != nullptr)
                {
                    baseIterator_ = other.baseIterator_->clone();
                }

                return *this;
            }

            ConstIterator<T> & operator++(void) HTD_OVERRIDE
            {
                if (baseIterator_!= nullptr)
                {
                    ++(*baseIterator_);
                }

                return *this;
            }

            ConstIterator<T> operator++(int)
            {
                ConstIterator<T> ret(*baseIterator_);

                operator++();

                return ret;
            }

            ConstIterator<T> & operator=(const ConstIterator<T> & other)
            {
                if (baseIterator_!= nullptr)
                {
                    delete baseIterator_;
                }

                baseIterator_ = other.baseIterator_->clone();

                return *this;
            }

            bool operator==(const htd::ConstIteratorBase<T> & other) const HTD_OVERRIDE
            {
                bool ret = false;

                const ConstIterator<T> * o = dynamic_cast<const ConstIterator<T> *>(&other);

                if (o != nullptr)
                {
                    if (baseIterator_ == nullptr)
                    {
                        ret = o->baseIterator_ == nullptr;
                    }
                    else if (o->baseIterator_ != nullptr)
                    {
                        ret = *baseIterator_ == *(o->baseIterator_ );
                    }
                }

                return ret;
            }

            bool operator==(const ConstIterator<T> & other) const
            {
                bool ret = false;

                if (baseIterator_ == nullptr)
                {
                    ret = other.baseIterator_ == nullptr;
                }
                else if (other.baseIterator_ != nullptr)
                {
                    ret = *baseIterator_ == *(other.baseIterator_ );
                }

                return ret;
            }

            bool operator!=(const htd::ConstIteratorBase<T> & other) const HTD_OVERRIDE
            {
                return !(*this == other);
            }

            bool operator!=(const ConstIterator<T> & other) const
            {
                return !(*this == other);
            }

            const T * operator->(void) const HTD_OVERRIDE
            {
                return &(*(*(baseIterator_)));
            }

            const T & operator*(void) const HTD_OVERRIDE
            {
                return *(*baseIterator_);
            }

            ConstIterator<T> * clone(void) const HTD_OVERRIDE
            {
                ConstIterator<T> * ret = nullptr;

                if (baseIterator_ == nullptr)
                {
                    return new ConstIterator<T>();
                }
                else
                {
                    return new ConstIterator<T>(*this);
                }

                return ret;
            }

        private:
            htd::ConstIteratorBase<T> * baseIterator_;

            ConstIterator<T>(const htd::ConstIteratorBase<T> & original) : baseIterator_(original.clone())
            {

            }
    };
}

#endif /* HTD_HTD_CONSTITERATOR_HPP */