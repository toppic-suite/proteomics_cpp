//
// $Id: MemoryIndex.hpp 1621 2009-12-15 17:43:15Z chambm $
//
//
// Original author: Matt Chambers <matt.chambers .@. vanderbilt.edu>
//
// Copyright 2009 Vanderbilt University - Nashville, TN 37232
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.
//

#ifndef _MEMORYINDEX_HPP_
#define _MEMORYINDEX_HPP_


#include "Index.hpp"


namespace pwiz {
namespace data {


/// index implementation in memory;
/// find(string id) is O(logN);
/// find(ordinal index) is O(1);
/// memory footprint is basically (number of entries * 2)
class PWIZ_API_DECL MemoryIndex : public Index
{
    public:

    MemoryIndex();

    virtual void create(std::vector<Entry>& entries);
    virtual size_t size() const;
    virtual EntryPtr find(const std::string& id) const;
    virtual EntryPtr find(size_t index) const;

    private:
    class Impl;
    boost::shared_ptr<Impl> impl_;
};


} // namespace data
} // namespace pwiz


#endif // _MEMORYINDEX_HPP_
