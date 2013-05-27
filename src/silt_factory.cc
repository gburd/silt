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

#include "silt_factory.h"
#include "hash_table_default.h"
#include "hash_table_cuckoo.h"
#include "file_store.h"
#include "silt.h"
#include "silt_sf.h"
#include "silt_sf_ordered_trie.h"
#include "silt_partition.h"
#include "silt_combi.h"
#include "sorter.h"
#include "silt_proxy.h"
#include "silt_monitor.h"
#include "bdb.h"
#include "debug.h"

namespace silt {

    Silt *
    Silt_Factory::New(std::string config_file)
    {
        DPRINTF(2, "Silt_Factory::New(): use config file: %s\n", config_file.c_str());
        Configuration *config = new Configuration(config_file);
        return Silt_Factory::New(config);
    }

    Silt *
    Silt_Factory::New(const Configuration *config)
    {
        std::string type = config->GetStringValue("child::type");
        DPRINTF(2, "Silt_Factory::New(): creating a new instance of type=%s\n", type.c_str());
        Silt *result = NULL;

        if (type == "file")
            result = new FileStore();
        else if (type == "sf")
            result = new Silt_SF();
        else if (type == "sf_ordered_trie")
            result = new Silt_SF_Ordered_Trie();
        else if (type == "partition")
            result = new Silt_Partition();
        else if (type == "combi")
            result = new Silt_Combi();
        else if (type == "default")
            result = new HashTableDefault();
        else if (type == "cuckoo")
            result = new HashTableCuckoo();
        else if (type == "sorter")
            result = new Sorter();
        else if (type == "proxy")
            result = new Silt_Proxy();
        else if (type == "monitor")
            result = new Silt_Monitor();

#ifdef HAVE_LIBDB
        else if (type == "bdb")
            result = new BDB();

#endif

        if (!result)
            return NULL;

        result->SetConfig(config);
        return result;
    }

} // namespace silt
