/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */

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

    Silt*
    Silt_Factory::New(std::string config_file)
    {
        DPRINTF(2, "Silt_Factory::New(): use config file: %s\n", config_file.c_str());
        Configuration* config = new Configuration(config_file);
        return Silt_Factory::New(config);
    }

    Silt*
    Silt_Factory::New(const Configuration* config)
    {
        std::string type = config->GetStringValue("child::type");
        DPRINTF(2, "Silt_Factory::New(): creating a new instance of type=%s\n", type.c_str());
        Silt* result = NULL;

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
