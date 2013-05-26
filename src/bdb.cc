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

#include "bdb.h"
#include "configuration.h"
#include <cassert>
#include <sstream>

#ifdef HAVE_LIBDB

namespace silt {

    BDB::BDB()
        : dbp_(NULL)
    {
    }

    BDB::~BDB()
    {
        if (dbp_)
            Close();
    }

    Silt_Return
    BDB::Create()
    {
        if (dbp_)
            return ERROR;

        // TODO: use DB_ENV to optimize BDB configuration settings
        // TODO: optimize for SSDs

        int ret = db_create(&dbp_, NULL, 0);
        if (ret != 0) {
            fprintf(stderr, "BDB::Create(): Error while creating DB: %s\n", db_strerror(ret));
            return ERROR;
        }

        std::string filename = config_->GetStringValue("child::file") + "_";
        filename += config_->GetStringValue("child::id");

        ret = dbp_->open(dbp_, NULL, filename.c_str(), NULL, DB_BTREE, DB_THREAD | DB_CREATE | DB_TRUNCATE, 0);
        if (ret != 0) {
            fprintf(stderr, "BDB::Create(): Error while creating DB: %s\n", db_strerror(ret));
            dbp_->close(dbp_, 0);
            dbp_ = NULL;
            return ERROR;
        }

        size_ = 0;

        return OK;
    }

    Silt_Return
    BDB::Open()
    {
        if (dbp_)
            return ERROR;

        int ret = db_create(&dbp_, NULL, 0);
        if (ret != 0) {
            fprintf(stderr, "BDB::Open(): Error while creating DB: %s\n", db_strerror(ret));
            return ERROR;
        }

        std::string filename = config_->GetStringValue("child::file") + "_";
        filename += config_->GetStringValue("child::id");

        ret = dbp_->open(dbp_, NULL, filename.c_str(), NULL, DB_BTREE, DB_THREAD, 0);
        if (ret != 0) {
            fprintf(stderr, "BDB::Open(): Error while creating DB: %s\n", db_strerror(ret));
            dbp_->close(dbp_, 0);
            dbp_ = NULL;
            return ERROR;
        }

        // TODO: load size_ from file
        size_ = 0;

        return OK;
    }

    Silt_Return
    BDB::ConvertTo(Silt* new_store) const
    {
        BDB* bdb = dynamic_cast<BDB*>(new_store);
        if (!bdb)
            return UNSUPPORTED;

        bdb->Close();

        std::string src_filename = config_->GetStringValue("child::file") + "_";
        src_filename += config_->GetStringValue("child::id");

        std::string dest_filename = bdb->config_->GetStringValue("child::file") + "_";
        dest_filename += bdb->config_->GetStringValue("child::id");

        if (unlink(dest_filename.c_str())) {
            fprintf(stderr, "BDB::ConvertTo(): cannot unlink the destination file\n");
        }
        if (link(src_filename.c_str(), dest_filename.c_str())) {
            fprintf(stderr, "BDB::ConvertTo(): cannot link the destination file\n");
        }

        if (bdb->Open() != OK)
            return ERROR;

        return OK;
    }

    Silt_Return
    BDB::Flush()
    {
        if (!dbp_)
            return ERROR;

        // TODO: implement
        return OK;
    }

    Silt_Return
    BDB::Close()
    {
        if (!dbp_)
            return ERROR;

        dbp_->close(dbp_, 0);
        dbp_ = NULL;

        return OK;
    }

    Silt_Return
    BDB::Destroy()
    {
        if (dbp_)
            return ERROR;

        DB* dbp;

        int ret = db_create(&dbp, NULL, 0);
        if (ret != 0) {
            fprintf(stderr, "BDB::Destroy(): Error while creating DB: %s\n", db_strerror(ret));
            return ERROR;
        }

        std::string filename = config_->GetStringValue("child::file") + "_";
        filename += config_->GetStringValue("child::id");

        ret = dbp->remove(dbp, filename.c_str(), NULL, 0);
        if (ret != 0) {
            fprintf(stderr, "BDB::Destroy(): Error while removing DB: %s\n", db_strerror(ret));
            dbp->close(dbp, 0);
            return ERROR;
        }

        dbp->close(dbp, 0);
        return OK;
    }

    Silt_Return
    BDB::Status(const Silt_StatusType& type, Value& status) const
    {
        if (!dbp_)
            return ERROR;

        std::ostringstream oss;
        switch (type) {
        case NUM_DATA:
        case NUM_ACTIVE_DATA:
            oss << size_;
            break;
		case CAPACITY:
            oss << -1;      // unlimited
            break;
        case MEMORY_USE:
            {
                uint32_t gbytes = 0;
                uint32_t bytes = 0;
                int ncache = 0;
                int ret = dbp_->get_cachesize(dbp_, &gbytes, &bytes, &ncache);
                if (ret != 0) {
                    fprintf(stderr, "BDB::Status(): Error while querying DB: %s\n", db_strerror(ret));
                    return ERROR;
                }
                oss << gbytes * (1024 * 1024 * 1024) + bytes;   // BDB uses powers of two
            }
            break;
        case DISK_USE:
            {
                // TODO: check if this work
                DB_BTREE_STAT stat;
                int ret = dbp_->stat(dbp_, NULL, &stat, DB_FAST_STAT);
                if (ret != 0) {
                    fprintf(stderr, "BDB::Status(): Error while querying DB: %s\n", db_strerror(ret));
                    return ERROR;
                }
                oss << stat.bt_pagecnt * stat.bt_pagesize;
            }
            break;
        default:
            return UNSUPPORTED;
        }
        status = NewValue(oss.str());
        return OK;
    }

    Silt_Return
    BDB::Put(const ConstValue& key, const ConstValue& data)
    {
        if (!dbp_)
            return ERROR;

        if (key.size() == 0)
            return INVALID_KEY;

        DBT key_v;
        memset(&key_v, 0, sizeof(DBT));
        key_v.data = const_cast<char*>(key.data());
        key_v.size = key.size();

        DBT data_v;
        memset(&data_v, 0, sizeof(DBT));
        data_v.data = const_cast<char*>(data.data());
        data_v.size = data.size();

        int ret = dbp_->put(dbp_, NULL, &key_v, &data_v, DB_NOOVERWRITE);
        if (ret == 0) {
            ++size_;
            return OK;
        }
        else if (ret == DB_KEYEXIST)
            ret = dbp_->put(dbp_, NULL, &key_v, &data_v, 0);

        if (ret != 0) {
            fprintf(stderr, "BDB::Put(): %s\n", db_strerror(ret));
            return ERROR;
        }

        return OK;
    }

    Silt_Return
    BDB::Delete(const ConstValue& key)
    {
        if (!dbp_)
            return ERROR;

        if (key.size() == 0)
            return INVALID_KEY;

        DBT key_v;
        memset(&key_v, 0, sizeof(DBT));
        key_v.data = const_cast<char*>(key.data());
        key_v.size = key.size();

        int ret = dbp_->del(dbp_, NULL, &key_v, 0);
        if (ret == 0) {
            --size_;
            return OK;
        }
        else if (ret == DB_NOTFOUND)
            return KEY_NOT_FOUND;
        if (ret != 0) {
            fprintf(stderr, "BDB::Delete(): %s\n", db_strerror(ret));
            return ERROR;
        }

        return OK;
    }

    Silt_Return
    BDB::Contains(const ConstValue& key) const
    {
        if (!dbp_)
            return ERROR;

        if (key.size() == 0)
            return INVALID_KEY;

        DBT key_v;
        memset(&key_v, 0, sizeof(DBT));
        key_v.data = const_cast<char*>(key.data());
        key_v.size = key.size();

        int ret = dbp_->exists(dbp_, NULL, &key_v, 0);
        if (ret == 0)
            return OK;
        else if (ret == DB_NOTFOUND)
            return KEY_NOT_FOUND;
        else {
            fprintf(stderr, "BDB::Contains(): %s\n", db_strerror(ret));
            return ERROR;
        }
    }

    Silt_Return
    BDB::Length(const ConstValue& key, size_t& len) const
    {
        if (!dbp_)
            return ERROR;

        if (key.size() == 0)
            return INVALID_KEY;

        DBT key_v;
        memset(&key_v, 0, sizeof(DBT));
        key_v.data = const_cast<char*>(key.data());
        key_v.size = key.size();

        DBT data_v;
        memset(&data_v, 0, sizeof(DBT));
        data_v.flags = DB_DBT_USERMEM;

        int ret = dbp_->get(dbp_, NULL, &key_v, &data_v, 0);
        if (ret == 0 || ret == DB_BUFFER_SMALL) {
            len = data_v.size;
            return OK;
        }
        else if (ret == DB_NOTFOUND)
            return KEY_NOT_FOUND;
        else {
            fprintf(stderr, "BDB::Length(): %s\n", db_strerror(ret));
            return ERROR;
        }
    }

    Silt_Return
    BDB::Get(const ConstValue& key, Value& data, size_t offset, size_t len) const
    {
        if (!dbp_)
            return ERROR;

        if (key.size() == 0)
            return INVALID_KEY;

        size_t data_len = 0;
        Silt_Return ret_len = Length(key, data_len);
        if (ret_len != OK)
            return ret_len;

        if (offset > data_len)
            return END;

        if (offset + len > data_len)
            len = data_len - offset;

        data.resize(len, false);

        DBT key_v;
        memset(&key_v, 0, sizeof(DBT));
        key_v.data = const_cast<char*>(key.data());
        key_v.size = key.size();

        DBT data_v;
        memset(&data_v, 0, sizeof(DBT));
        data_v.data = data.data();
        data_v.ulen = len;
        data_v.flags = DB_DBT_USERMEM;

        int ret = dbp_->get(dbp_, NULL, &key_v, &data_v, 0);
        if (ret != 0) {
            fprintf(stderr, "BDB::Get(): %s\n", db_strerror(ret));
            return ERROR;
        }
        return OK;
    }

    Silt_ConstIterator
    BDB::Enumerate() const
    {
        if (!dbp_)
            return Silt_ConstIterator();

        IteratorElem* elem = new IteratorElem(this);

        int ret = dbp_->cursor(dbp_, NULL, &elem->cursor, 0);
        if (ret != 0) {
            fprintf(stderr, "BDB::Enumerate(): %s\n", db_strerror(ret));
            return Silt_ConstIterator();
        }

        elem->Increment(true);

        return Silt_ConstIterator(elem);
    }

    Silt_Iterator
    BDB::Enumerate()
    {
        if (!dbp_)
            return Silt_Iterator();

        IteratorElem* elem = new IteratorElem(this);

        int ret = dbp_->cursor(dbp_, NULL, &elem->cursor, 0);
        if (ret != 0) {
            fprintf(stderr, "BDB::Enumerate(): %s\n", db_strerror(ret));
            return Silt_Iterator();
        }

        elem->Increment(true);

        return Silt_Iterator(elem);
    }

    Silt_ConstIterator
    BDB::Find(const ConstValue& key) const
    {
        if (!dbp_)
            return Silt_ConstIterator();

        if (key.size() == 0)
            return Silt_ConstIterator();

        IteratorElem* elem = new IteratorElem(this);

        elem->key = key;

        int ret = dbp_->cursor(dbp_, NULL, &elem->cursor, 0);
        if (ret != 0) {
            fprintf(stderr, "BDB::Find(): %s\n", db_strerror(ret));
            delete elem;
            return Silt_ConstIterator();
        }

        elem->Increment(true);

        return Silt_ConstIterator(elem);
    }

    Silt_Iterator
    BDB::Find(const ConstValue& key)
    {
        if (!dbp_)
            return Silt_Iterator();

        if (key.size() == 0)
            return Silt_Iterator();

        IteratorElem* elem = new IteratorElem(this);

        elem->key = key;

        int ret = dbp_->cursor(dbp_, NULL, &elem->cursor, 0);
        if (ret != 0) {
            fprintf(stderr, "BDB::Find(): %s\n", db_strerror(ret));
            delete elem;
            return Silt_Iterator();
        }

        elem->Increment(true);

        return Silt_Iterator(elem);
    }

    BDB::IteratorElem::IteratorElem(const BDB* silt)
    {
        this->silt = silt;
    }

    BDB::IteratorElem::~IteratorElem()
    {
        cursor->close(cursor);
        cursor = NULL;
    }

    Silt_IteratorElem*
    BDB::IteratorElem::Clone() const
    {
        IteratorElem* elem = new IteratorElem(static_cast<const BDB*>(silt));
        *elem = *this;
        int ret = cursor->dup(cursor, &elem->cursor, DB_POSITION);
        if (ret != 0) {
            fprintf(stderr, "BDB::Clone(): %s\n", db_strerror(ret));
            return NULL;
        }
        return elem;
    }

    void
    BDB::IteratorElem::Next()
    {
        Increment(false);
    }

    void
    BDB::IteratorElem::Increment(bool initial)
    {
        int flags;

        DBT key_v;
        memset(&key_v, 0, sizeof(DBT));
        DBT data_v;
        memset(&data_v, 0, sizeof(DBT));

        if (initial) {
            if (key.size() == 0) {
                flags = DB_FIRST;
                key_v.flags = DB_DBT_USERMEM;
                data_v.flags = DB_DBT_USERMEM;
            }
            else {
                flags = DB_SET;
                key_v.data = key.data();
                key_v.size = key.size();
                data_v.flags = DB_DBT_USERMEM;
            }
        }
        else {
            key_v.flags = DB_DBT_USERMEM;
            data_v.flags = DB_DBT_USERMEM;
            flags = DB_NEXT;
        }

        // obtain the length of key & data
        int ret = cursor->get(cursor, &key_v, &data_v, flags);
        if (ret == 0) {
            // this should not happen because we have key_v.ulen = 0, and there is no zero-length key
            assert(false);
            state = END;
            return;
        }
        else if (ret == DB_NOTFOUND) {
            state = END;
            return;
        }
        else if (ret != DB_BUFFER_SMALL) {
            fprintf(stderr, "BDB::IteratorElem::Increment(): Error while obtaining length: %s\n", db_strerror(ret));
            state = END;
            return;
        }

        // TODO: DPRINTF/fprintf(stderr, "%d %d\n", key_v.size, data_v.size);

        // retrieve key & data
        key.resize(key_v.size, false);
        key_v.data = key.data();
        key_v.size = key.size();
        key_v.ulen = key_v.size;
        key_v.flags = DB_DBT_USERMEM;

        data.resize(data_v.size, false);
        data_v.data = data.data();
        data_v.size = data.size();
        data_v.ulen = data_v.size;
        key_v.flags = DB_DBT_USERMEM;

        ret = cursor->get(cursor, &key_v, &data_v, flags);
        if (ret != 0) {
            fprintf(stderr, "BDB::IteratorElem::Increment(): Error while reading key and data: %s\n", db_strerror(ret));
            state = END;
            return;
        }

        state = OK;
        key.resize(key_v.size);
        data.resize(data_v.size);
    }

}  // namespace silt

#endif
