// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>

#include "runtime/stream_load_pipe.h" // for StreamLoadPipe
#include "util/uid_util.h" // for std::hash for UniqueId

namespace palo {

// used to register all streams in process so that other module can get this stream
class LoadStreamMgr {
public:
    LoadStreamMgr() { }
    ~LoadStreamMgr() { }

    Status put(const UniqueId& id,
               std::shared_ptr<StreamLoadPipe> stream) {
        std::lock_guard<std::mutex> l(_lock);
        auto it = _stream_map.find(id);
        if (it != std::end(_stream_map)) {
            return Status("id already exist");
        }
        _stream_map.emplace(id, stream);
        return Status::OK;
    }
    
    std::shared_ptr<StreamLoadPipe> get(const UniqueId& id) {
        std::lock_guard<std::mutex> l(_lock);
        auto it = _stream_map.find(id);
        if (it == std::end(_stream_map)) {
            return nullptr;
        }
        auto stream = it->second;
        _stream_map.erase(it);
        return stream;
    }

private:
    std::mutex _lock;
    std::unordered_map<UniqueId, std::shared_ptr<StreamLoadPipe>> _stream_map;
};

}