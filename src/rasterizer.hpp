//
// Created by Vyacheslav Zhdanovskiy <zhdanovskiy@iitp.ru> on 9/30/19.
//

#ifndef RASTERIZER_HPP
#define RASTERIZER_HPP

#include "tty_context.hpp"
#include "screen_buffer.hpp"
#include "math.hpp"
#include "mesh.hpp"
#include "thread_pool.hpp"
#include "batch_tasks.hpp"
#include <algorithm>

namespace rst
{

template<typename VShader, typename FShader>
class Rasterizer
{
public:
    using VsIn  = typename VShader::InType;
    using VsOut = typename VShader::OutType;
    using FsIn  = typename FShader::InType;

    static constexpr std::size_t VERTEX_BATCH_SIZE{2048};
    static constexpr std::size_t RASTER_TRI_BATCH_SIZE{2048};
    static constexpr std::size_t FRAGMENT_BATCH_SIZE{2048};

         Rasterizer(TtyContext &context, VShader &vs, FShader &fs,
                    std::size_t threads = 1)                        noexcept;
    void RasterizeVertexArray(const std::vector<VsIn> &vertices,
                              const std::vector<unsigned> &indices) noexcept;
private:
    using VbTask = VertexBatchTask<VShader>;
    using VbTaskParams = typename VbTask::ThreadParams;
    using RastTask = RasterBatchTask<VShader>;
    using RastTaskParams = typename RastTask::ThreadParams;
    using FragTask = FragBatchTask<typename RastTask::Output, FShader>;
    using FragTaskParams = typename FragTask::ThreadParams;

    TtyContext  &m_context;
    FrameBuffer &m_frameBuf;
    DepthBuffer &m_depthBuf;
    VShader     &m_vertexShader;
    FShader     &m_fragmentShader;
    std::size_t m_threads;
    Culling     m_culling;

    std::vector<VsOut> m_vsOutput;
    std::vector<VbTaskParams> m_vbTaskParams;
    std::vector<RastTaskParams> m_rastTaskParams;
    std::vector<FragTaskParams> m_fragTaskParams;
};

template<typename VShader, typename FShader>
Rasterizer<VShader, FShader>::Rasterizer(TtyContext &context, VShader &vs, FShader &fs, std::size_t threads) noexcept:
    m_context{context},
    m_frameBuf{context.GetFrameBuffer()},
    m_depthBuf{context.GetDepthBuffer()},
    m_vertexShader{vs},
    m_fragmentShader{fs},
    m_threads{threads},
    m_culling{Culling::Ccw}
{
    for (auto i = 0ul; i < m_threads; ++i)
    {
        m_vbTaskParams.emplace_back(VbTaskParams{m_vertexShader, m_vsOutput});
        m_rastTaskParams.emplace_back(RastTaskParams{m_vsOutput, m_culling, m_frameBuf});
        m_fragTaskParams.emplace_back(FragTaskParams{m_fragmentShader, m_frameBuf, m_depthBuf, context.GetScreenLock()});
    }
}

template<typename VShader, typename FShader>
void Rasterizer<VShader, FShader>::RasterizeVertexArray(const std::vector<VsIn> &vertices,
                                                        const std::vector<unsigned> &indices) noexcept
{
    m_vsOutput.reserve(vertices.size());
    /*for (auto &param : m_rastTaskParams)
    {
        param.output.reserve(1920*1080);
    }*/
    {
        ThreadPool<VbTask> vbPool{m_threads, m_vbTaskParams};
        for (auto i = 0ul; i < vertices.size(); i += VERTEX_BATCH_SIZE)
        {
            auto end = std::min(vertices.size(), i + VERTEX_BATCH_SIZE);
            vbPool.EnqueueTask(VbTask{&vertices[0], i, end});
        }
    }

    {
        ThreadPool<RastTask> rastPool{m_threads, m_rastTaskParams};
        for (auto i = 0ul; i < indices.size(); i += RASTER_TRI_BATCH_SIZE * 3)
        {
            auto end = std::min(indices.size(), i + RASTER_TRI_BATCH_SIZE * 3);
            rastPool.EnqueueTask(RastTask{&indices[0], i, end});
        }
    }

    {
        ThreadPool<FragTask> fragPool{m_threads, m_fragTaskParams};
        for (auto &rastOut : m_rastTaskParams)
        {
            for (auto i = 0ul; i < rastOut.output.size(); i += FRAGMENT_BATCH_SIZE)
            {
                auto end = std::min(rastOut.output.size(), i + FRAGMENT_BATCH_SIZE);
                fragPool.EnqueueTask(FragTask{&rastOut.output[0], i, end});
            }
        }
    }

    m_vsOutput.clear();
    for (auto &params : m_rastTaskParams)
    {
        params.output.clear();
    }
}

}

#endif //RASTERIZER_HPP
