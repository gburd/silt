/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#ifndef _SILT_FACTORY_H_
#define _SILT_FACTORY_H_

//using silt::DataHeader;
//using silt::HashUtil;

//using namespace std;

#include "configuration.h"
#include "silt.h"

namespace silt {

    class Silt_Factory {
    public:
        static Silt* New(std::string config_file);
        static Silt* New(const Configuration* config);
    };

} // namespace silt

#endif  // #ifndef _SILT_FACTORY_H_
