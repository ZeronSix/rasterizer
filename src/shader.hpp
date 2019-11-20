//
// Created by Vyacheslav Zhdanovskiy <zeronsix@gmail.com> on 11/17/19.
//

#ifndef SHADER_HPP
#define SHADER_HPP

namespace rst
{

template<typename VsInType, typename VsOutType>
struct Shader
{
public:
    using VsIn  = VsInType;
    using VsOut = VsOutType;
};

}

#endif //SHADER_HPP
