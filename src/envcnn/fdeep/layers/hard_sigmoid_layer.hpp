// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "activation_layer.hpp"
#include "src/envcnn/fdeep/recurrent_ops.hpp"

#include <algorithm>
#include <string>

namespace fdeep { namespace internal
{

class hard_sigmoid_layer : public activation_layer
{
public:
    explicit hard_sigmoid_layer(const std::string& name)
        : activation_layer(name)
    {
    }
protected:
    tensor5 transform_input(const tensor5& in_vol) const override
    {
        return transform_tensor5(hard_sigmoid_activation, in_vol);
    }
};

} } // namespace fdeep, namespace internal
