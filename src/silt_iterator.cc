/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/*-
 * SILT: A Memory-Efficient, High-Performance Key-Value Store
 *
 * Copyright © 2011 Carnegie Mellon University
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
 * EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT. THIS SOFTWARE IS
 * PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO
 * OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
 * MODIFICATIONS.
 *
 * GOVERNMENT USE: If you are acquiring this software on behalf of the
 * U.S. government, the Government shall have only "Restricted Rights" in the
 * software and related documentation as defined in the Federal Acquisition
 * Regulations (FARs) in Clause 52.227.19 (c) (2). If you are acquiring the
 * software on behalf of the Department of Defense, the software shall be
 * classified as "Commercial Computer Software" and the Government shall have
 * only "Restricted Rights" as defined in Clause 252.227-7013 (c) (1) of
 * DFARs. Notwithstanding the foregoing, the authors grant the U.S. Government
 * and others acting in its behalf permission to use and distribute the
 * software in accordance with the terms specified in this license.
 */

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

    Silt_IteratorElem *
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
    Silt_IteratorElem::Replace(const ConstValue &data)
    {
        (void)data;
        return UNSUPPORTED;
    }

    Silt_Return
    Silt_IteratorElem::Replace(const ConstValue &key, const ConstValue &data)
    {
        (void)key;
        (void)data;
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

    Silt_ConstIterator::Silt_ConstIterator(Silt_IteratorElem *elem)
        : elem(elem)
    {
        assert(elem);
    }

    Silt_ConstIterator::Silt_ConstIterator(const Silt_ConstIterator &it)
        : elem(NULL)
    {
        *this = it;
    }

    Silt_ConstIterator::~Silt_ConstIterator()
    {
        delete elem;
        elem = NULL;
    }

    Silt_ConstIterator &
    Silt_ConstIterator::operator=(const Silt_ConstIterator &rhs)
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

    Silt_ConstIterator &
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

    Silt_ConstIterator &
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

    const Silt_IteratorElem &
    Silt_ConstIterator::operator*() const
    {
        assert(elem);
        return *elem;
    }

    const Silt_IteratorElem *
    Silt_ConstIterator::operator->() const
    {
        assert(elem);
        return elem;
    }

    Silt_Iterator::Silt_Iterator()
        : Silt_ConstIterator()
    {
    }

    Silt_Iterator::Silt_Iterator(Silt_IteratorElem *elem)
        : Silt_ConstIterator(elem)
    {
    }

    Silt_Iterator::Silt_Iterator(const Silt_Iterator &it)
        : Silt_ConstIterator()
    {
        *this = it;
    }

    Silt_Iterator &
    Silt_Iterator::operator=(const Silt_Iterator &rhs)
    {
        return (*this = static_cast<const Silt_ConstIterator &>(rhs));
    }

    Silt_Iterator &
    Silt_Iterator::operator=(const Silt_ConstIterator &rhs)
    {
        // same as Silt_ConstIterator::operator=()
        delete elem;

        if (rhs.elem)
            elem = rhs.elem->Clone();
        else
            elem = NULL;

        return *this;
    }

    Silt_Iterator &
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

    Silt_Iterator &
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

    Silt_IteratorElem &
    Silt_Iterator::operator*()
    {
        assert(elem);
        return *elem;
    }

    Silt_IteratorElem *
    Silt_Iterator::operator->()
    {
        assert(elem);
        return elem;
    }

} // namespace silt
