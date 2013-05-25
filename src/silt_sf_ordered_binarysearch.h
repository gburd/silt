#ifndef _SILT_SF_ORDERED_BINARYSEARCH_H_
#define _SILT_SF_ORDERED_BINARYSEARCH_H_

#include "silt_factory.h"

namespace silt
{
    template <typename Store>
    class Silt_SF_Ordered_BinarySearch : public Silt
    {
    public:
        static Silt_SF_Ordered_BinarySearch<Store>* Create(Configuration* const &config, const uint64_t &hash_table_size) __attribute__ ((warn_unused_result));
        static Silt_SF_Ordered_BinarySearch<Store>* Open(Configuration* const &config) __attribute__ ((warn_unused_result));

    protected:
        Silt_SF_Ordered_BinarySearch(Configuration* const &config);
        void Initialize(uint64_t hash_table_size);
        void LoadFromFile();
        void StoreToFile();

    public:
        ~Silt_SF_Ordered_BinarySearch();

        int Insert(const char* const &key, const uint32_t &key_len, const char* const &data, const uint32_t &data_len) __attribute__ ((warn_unused_result));
        int Get(const char* const &key, const uint32_t &key_len, char* &data, uint32_t &data_len) __attribute__ ((warn_unused_result));
        int Delete(const char* const &key, const uint32_t &key_len) __attribute__ ((warn_unused_result));
        //int DoForEach(void (*callForEach)(const uint32_t &key_len, const uint32_t &data_len, const char* const &key, const char* const &data, const bool &isDeleted), void (*callBeforeRest)()) const __attribute__ ((warn_unused_result));
        int Flush() __attribute__ ((warn_unused_result));
        int Destroy() __attribute__ ((warn_unused_result));

        SiltIter IteratorBegin() const __attribute__ ((warn_unused_result));

    protected:
        string id_;
        Configuration* config_;

        Store* data_store_;

    protected:
        std::size_t lookup_key_index(const char* key, uint64_t off, int64_t n) const;

        void finalize();

        std::size_t hash_state() const;

    private:
        bool initialized_;
        bool finalized_;

        uint32_t key_len_;
        uint32_t data_len_;

        off_t base_data_store_offset_;
        uint64_t hash_table_size_;
        uint64_t actual_size_;

        char* last_inserted_key_;   // for validating new keys
    };
}  // namespace silt

#endif  // #ifndef _SILT_SF_ORDERED_BINARYSEARCH_H_

