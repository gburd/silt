/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include "silt_iterator.h"
#include "silt.h"
#include "debug.h"

#include <cassert>

namespace silt {
    class Silt;
    class SiltIter;

	Silt_IteratorElem::Silt_IteratorElem()
		: state(END), silt(NULL)
	{
	}

	Silt_IteratorElem::~Silt_IteratorElem()
   	{
   	}

    Silt_IteratorElem*
	Silt_IteratorElem::Clone() const
	{
	   	return NULL;
   	}

    void
	Silt_IteratorElem::Prev()
	{
        state = UNSUPPORTED;
   	}

    void
	Silt_IteratorElem::Next()
	{
        state = UNSUPPORTED;
   	}

    Silt_Return
    Silt_IteratorElem::Replace(const ConstValue& data)
    {
        (void)data;
        return UNSUPPORTED;
    }

    Silt_Return
    Silt_IteratorElem::Replace(const ConstValue& key, const ConstValue& data)
    {
        (void)key; (void)data;
        return UNSUPPORTED;
    }

    Silt_Return
    Silt_IteratorElem::Delete()
    {
        return UNSUPPORTED;
    }

	Silt_ConstIterator::Silt_ConstIterator()
		: elem(NULL)
	{
	}

    Silt_ConstIterator::Silt_ConstIterator(Silt_IteratorElem* elem)
	   	: elem(elem)
   	{
	   	assert(elem);
   	}

	Silt_ConstIterator::Silt_ConstIterator(const Silt_ConstIterator& it)
	   	: elem(NULL)
   	{
	   	*this = it;
   	}

	Silt_ConstIterator::~Silt_ConstIterator()
   	{
	   	delete elem;
	   	elem = NULL;
   	}

	Silt_ConstIterator&
	Silt_ConstIterator::operator=(const Silt_ConstIterator& rhs)
   	{
	   	delete elem;
	   	if (rhs.elem)
		   	elem = rhs.elem->Clone();
	   	else
		   	elem = NULL;
	   	return *this;
   	}

    bool
    Silt_ConstIterator::IsOK() const
    {
        return elem && elem->state == OK;
    }

    bool
    Silt_ConstIterator::IsError() const
    {
        return elem && elem->state == ERROR;
    }

    bool
    Silt_ConstIterator::IsUnsupported() const
    {
        return elem && elem->state == UNSUPPORTED;
    }

    bool
    Silt_ConstIterator::IsKeyDeleted() const
    {
        return elem && elem->state == KEY_DELETED;
    }

    bool
    Silt_ConstIterator::IsKeyNotFound() const
    {
        return elem && elem->state == KEY_NOT_FOUND;
    }

    bool
    Silt_ConstIterator::IsInvalidKey() const
    {
        return elem && elem->state == INVALID_KEY;
    }

    bool
    Silt_ConstIterator::IsInvalidData() const
    {
        return elem && elem->state == INVALID_DATA;
    }

    bool
    Silt_ConstIterator::IsInvalidLength() const
    {
        return elem && elem->state == INVALID_LENGTH;
    }

    /*
    bool
    Silt_ConstIterator::IsUnused() const
    {
        return elem && elem->state == UNUSED;
    }
    */

    bool
    Silt_ConstIterator::IsEnd() const
    {
        return !elem || elem->state == END;
    }

	Silt_ConstIterator&
	Silt_ConstIterator::operator--()
   	{
	   	assert(elem);
	   	elem->Prev();
	   	return *this;
   	}

	Silt_ConstIterator
	Silt_ConstIterator::operator--(int)
   	{
	   	Silt_ConstIterator tmp(*this);
	   	operator--();
	   	return tmp;
   	}

	Silt_ConstIterator&
	Silt_ConstIterator::operator++()
   	{
	   	assert(elem);
	   	elem->Next();
	   	return *this;
   	}

	Silt_ConstIterator
	Silt_ConstIterator::operator++(int)
   	{
	   	Silt_ConstIterator tmp(*this);
	   	operator++();
	   	return tmp;
   	}

	const Silt_IteratorElem&
    Silt_ConstIterator::operator*() const
   	{
	   	assert(elem);
	   	return *elem;
   	}

	const Silt_IteratorElem*
    Silt_ConstIterator::operator->() const
   	{
	   	assert(elem);
	   	return elem;
   	}

	Silt_Iterator::Silt_Iterator()
		: Silt_ConstIterator()
	{
	}

    Silt_Iterator::Silt_Iterator(Silt_IteratorElem* elem)
		: Silt_ConstIterator(elem)
   	{
   	}

	Silt_Iterator::Silt_Iterator(const Silt_Iterator& it)
		: Silt_ConstIterator()
   	{
	   	*this = it;
   	}

	Silt_Iterator&
	Silt_Iterator::operator=(const Silt_Iterator& rhs)
   	{
	   	return (*this = static_cast<const Silt_ConstIterator&>(rhs));
   	}

	Silt_Iterator&
	Silt_Iterator::operator=(const Silt_ConstIterator& rhs)
   	{
        // same as Silt_ConstIterator::operator=()
	   	delete elem;
	   	if (rhs.elem)
		   	elem = rhs.elem->Clone();
	   	else
		   	elem = NULL;
	   	return *this;
   	}

	Silt_Iterator&
	Silt_Iterator::operator--()
   	{
	   	assert(elem);
	   	elem->Prev();
	   	return *this;
   	}

	Silt_Iterator
	Silt_Iterator::operator--(int)
   	{
	   	Silt_Iterator tmp(*this);
	   	operator--();
	   	return tmp;
   	}

	Silt_Iterator&
	Silt_Iterator::operator++()
   	{
	   	assert(elem);
	   	elem->Next();
	   	return *this;
   	}

	Silt_Iterator
	Silt_Iterator::operator++(int)
   	{
	   	Silt_Iterator tmp(*this);
	   	operator++();
	   	return tmp;
   	}

	Silt_IteratorElem&
    Silt_Iterator::operator*()
   	{
	   	assert(elem);
	   	return *elem;
   	}

	Silt_IteratorElem*
    Silt_Iterator::operator->()
   	{
	   	assert(elem);
	   	return elem;
   	}

} // namespace silt
