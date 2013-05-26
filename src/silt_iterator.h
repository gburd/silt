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
