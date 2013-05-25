/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#ifndef _SILT_ITERATOR_H_
#define _SILT_ITERATOR_H_

#include "basic_types.h"
#include "silt_types.h"
#include "value.h"
#include <iterator>

namespace silt {

    class Silt;

    struct Silt_IteratorElem {
        Silt_Return state;

        Value key;
        Value data;

        const Silt* silt;

        Silt_IteratorElem();
        virtual ~Silt_IteratorElem();

        virtual Silt_IteratorElem* Clone() const;

        virtual void Prev();
        virtual void Next();

        // the following methods may const_cast<Silt*>(silt)
        // because these are exposed as non-const to the user only by Silt_Iterator
        virtual Silt_Return Replace(const ConstValue& data);
        virtual Silt_Return Replace(const ConstValue& key, const ConstValue& data);
        virtual Silt_Return Delete();
    };

    struct Silt_ConstIterator : public std::iterator<std::bidirectional_iterator_tag, Silt_IteratorElem> {
        Silt_IteratorElem* elem;

        Silt_ConstIterator();
        ~Silt_ConstIterator();

        Silt_ConstIterator(Silt_IteratorElem* elem); // move
        Silt_ConstIterator(const Silt_ConstIterator& rhs);
        Silt_ConstIterator& operator=(const Silt_ConstIterator& rhs);

        bool IsOK() const;
        bool IsError() const;
        bool IsUnsupported() const;
        bool IsKeyDeleted() const;
        bool IsKeyNotFound() const;
        bool IsInvalidKey() const;
        bool IsInvalidData() const;
        bool IsInvalidLength() const;
        //bool IsUnused() const;
        bool IsEnd() const;

        Silt_ConstIterator& operator--();
        Silt_ConstIterator operator--(int);
        Silt_ConstIterator& operator++();
        Silt_ConstIterator operator++(int);

        const Silt_IteratorElem& operator*() const;
        const Silt_IteratorElem* operator->() const;
    };

    struct Silt_Iterator : public Silt_ConstIterator {
        Silt_Iterator();

        Silt_Iterator(Silt_IteratorElem* elem); // move
        Silt_Iterator(const Silt_Iterator& rhs);
        Silt_Iterator& operator=(const Silt_Iterator& rhs);
        Silt_Iterator& operator=(const Silt_ConstIterator& rhs);

        Silt_Iterator& operator--();
        Silt_Iterator operator--(int);
        Silt_Iterator& operator++();
        Silt_Iterator operator++(int);

        Silt_IteratorElem& operator*();
        Silt_IteratorElem* operator->();
    };

} // namespace silt

#endif  // #ifndef _SILT_ITERATOR_H_
