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

#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include <xercesc/util/XercesDefs.hpp>
#include <string>
#include <tbb/queuing_mutex.h>

XERCES_CPP_NAMESPACE_BEGIN
    class DOMDocument;
    class DOMXPathResult;
    class DOMElement;
XERCES_CPP_NAMESPACE_END

namespace silt {
    class ConfigurationObject;

    class Configuration {
    public:
        Configuration();
        Configuration(const std::string &config_file);
        Configuration(const Configuration* const &other, bool copy = false);
        ~Configuration();

        int CloneAndAppendNode(const std::string& source_gXPathExpression, const std::string& dest_gXPathExpression);
        Configuration* CloneAndAppendNodeAndGetConfig(const std::string& source_gXPathExpression, const std::string& dest_gXPathExpression);
        int CloneAndReplaceNode(const std::string& source_gXPathExpression, const std::string& dest_gXPathExpression);
        int CreateNodeAndAppend(const std::string& tagName, const std::string& dest_gXPathExpression);
        int DeleteNode(const std::string& gXPathExpression);
        int ExistsNode(const std::string& gXPathExpression) const;
        const std::string GetStringValue(const std::string& gXPathExpression) const;
        int SetStringValue(const std::string& gXPathExpression, const std::string& value);
        int WriteConfigFile(const std::string& config_file) const;
        int SetContextNode(const std::string& gXPathExpression);
        void ResetContextNode();
    private:
        XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* readConfigFile(const std::string& config_file);
        XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathResult* getXPathResult(const std::string& gXPathExpression) const;

        void SetContextNode(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* node);

        void initialize();
        void terminate();

        ConfigurationObject* conf_obj_;
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* context_node_;

        static tbb::queuing_mutex mutex_;
    };

} // namespace silt

#endif  // #ifndef _CONFIGURATION_H_
