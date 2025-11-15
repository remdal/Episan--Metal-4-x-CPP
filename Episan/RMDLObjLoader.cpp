/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLObjLoader.cpp            +++     +++		**/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 21/09/2025 15:22:51      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "RMDLObjLoader.hpp"

RMDLObjMesh::RMDLObjMesh()
{}

RMDLObjMesh::~RMDLObjMesh()
{}

int RMDLObjMesh::vertexCount()
{
    return (_vertexBuffer->length() / sizeof(RMDLObjVertex));
}

int RMDLObjMesh::indexCount()
{
    return (_indexBuffer->length() / sizeof(uint16_t));
}


RMDLObjLoader::RMDLObjLoader()
{}

RMDLObjLoader::~RMDLObjLoader()
{}

static constexpr uint kBufferSize = 2048;

RMDLObjLoader& RMDLObjLoader::initWithDevice( MTL::Device* device ) // == MTL::Device* pDevice ): _pDevice( pDevice->retain() )
{
    _device = device;
    return (*this);
}

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <tuple>
#include <optional>
#include <stdexcept>
#include <cstring>
#include <cmath>

#ifdef __APPLE__
// Optional Metal-cpp support. Define USE_METAL_CPP before including this header to enable.
#ifdef USE_METAL_CPP
#include <Metal/Metal.hpp>
#include <IOSurface/IOSurfaceRef.h>
#include <simd/simd.h>
#endif
#endif

namespace rmdl {

struct Vertex {
    float px, py, pz;
    float nx, ny, nz;
    float u, v;

    bool operator==(Vertex const &o) const noexcept {
        return std::memcmp(this, &o, sizeof(Vertex)) == 0;
    }
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices; // 32-bit indices
};

class RMDLObjLoader {
public:
    RMDLObjLoader() = default;

    // Load OBJ into Mesh (parsing only). Supports "v", "vn", "vt", and triangular or polygon faces.
    // Does basic deduplication of vertices.
    // Throws std::runtime_error on file or parse errors.
    Mesh loadObj(const std::string &path) const {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open OBJ file: " + path);
        }

        std::vector<std::array<float,3>> positions;
        std::vector<std::array<float,3>> normals;
        std::vector<std::array<float,2>> texcoords;

        // We fill this with unique vertices using a map from index-triple to output index.
        Mesh out;
        out.vertices.clear();
        out.indices.clear();

        // helper map: key = i/j/k triple encoded as string -> vertex index
        std::unordered_map<std::string, uint32_t> vertexCache;
        vertexCache.reserve(8192);

        std::string line;
        size_t lineNo = 0;
        while (std::getline(file, line)) {
            ++lineNo;
            if (line.empty()) continue;
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            if (prefix == "v") {
                float x,y,z; iss >> x >> y >> z;
                positions.push_back({x,y,z});
            } else if (prefix == "vn") {
                float x,y,z; iss >> x >> y >> z;
                normals.push_back({x,y,z});
            } else if (prefix == "vt") {
                float u,v; iss >> u >> v; texcoords.push_back({u,v});
            } else if (prefix == "f") {
                // Read the remainder and split by spaces to get vertex refs
                std::vector<std::string> verts;
                std::string token;
                while (iss >> token) verts.push_back(token);
                if (verts.size() < 3) {
                    throw std::runtime_error("Face with fewer than 3 verts at line " + std::to_string(lineNo));
                }
                // Triangulate polygon fan style
                for (size_t tri = 1; tri+1 < verts.size(); ++tri) {
                    std::array<std::string,3> faceRef = { verts[0], verts[tri], verts[tri+1] };
                    for (int k = 0; k < 3; ++k) {
                        const std::string &ref = faceRef[k];
                        // key is the raw token (handles negative indices too)
                        auto it = vertexCache.find(ref);
                        if (it != vertexCache.end()) {
                            out.indices.push_back(it->second);
                            continue;
                        }
                        // parse v/vt/vn. OBJ allows v, v/vt, v//vn, v/vt/vn
                        int vi = 0, vti = 0, vni = 0; // 1-based indices as in OBJ
                        parseObjVertexRef(ref, vi, vti, vni);
                        if (vi == 0) throw std::runtime_error("OBJ parse error: vertex index 0 at line " + std::to_string(lineNo));

                        std::array<float,3> P = {0,0,0};
                        std::array<float,3> N = {0,0,0};
                        std::array<float,2> T = {0,0};

                        // resolve indices (handles negative indices)
                        if (vi != 0) P = fetchPosition(positions, vi);
                        if (vni != 0) N = fetchNormal(normals, vni);
                        if (vti != 0) T = fetchTexcoord(texcoords, vti);

                        Vertex vtx;
                        vtx.px = P[0]; vtx.py = P[1]; vtx.pz = P[2];
                        vtx.nx = N[0]; vtx.ny = N[1]; vtx.nz = N[2];
                        vtx.u  = T[0]; vtx.v  = T[1];

                        uint32_t newIndex = static_cast<uint32_t>(out.vertices.size());
                        out.vertices.push_back(vtx);
                        out.indices.push_back(newIndex);
                        vertexCache.emplace(ref, newIndex);
                    }
                }
            }
            // skip other prefixes silently (o, g, s, mtllib, usemtl, etc.)
        }

        // If normals were missing, generate smooth normals
        if (normals.empty()) {
            generateNormals(out);
        }

        return out;
    }

#ifdef USE_METAL_CPP
    // Create Metal buffers (metal-cpp) from Mesh. Returns a tuple of (vertexBuffer, indexBuffer)
    // Caller keeps returned NS::SharedPtr references alive.
    std::pair<NS::SharedPtr<MTL::Buffer>, NS::SharedPtr<MTL::Buffer>>
    createMetalBuffers(const Mesh &mesh, MTL::Device *device, MTL::ResourceOptions options = MTL::ResourceCPUCacheModeDefaultCache) const {
        if (!device) throw std::runtime_error("device is null");
        size_t vSize = mesh.vertices.size() * sizeof(Vertex);
        size_t iSize = mesh.indices.size() * sizeof(uint32_t);

        NS::SharedPtr<MTL::Buffer> vbuf{device->newBuffer(vSize, options)};
        NS::SharedPtr<MTL::Buffer> ibuf{device->newBuffer(iSize, options)};
        if (!vbuf || !ibuf) throw std::runtime_error("Failed to create Metal buffers");

        std::memcpy(vbuf->contents(), mesh.vertices.data(), vSize);
        std::memcpy(ibuf->contents(), mesh.indices.data(), iSize);

        return {vbuf, ibuf};
    }
#endif

private:
    static void parseObjVertexRef(const std::string &ref, int &vi, int &vti, int &vni) {
        // Initialize
        vi = vti = vni = 0;
        // Token forms: v, v/vt, v//vn, v/vt/vn
        size_t firstSlash = ref.find('/');
        if (firstSlash == std::string::npos) {
            vi = std::stoi(ref);
            return;
        }
        // vi present
        vi = std::stoi(ref.substr(0, firstSlash));
        size_t secondSlash = ref.find('/', firstSlash + 1);
        if (secondSlash == std::string::npos) {
            // v/vt
            vti = std::stoi(ref.substr(firstSlash + 1));
            return;
        }
        // v//vn or v/vt/vn
        if (secondSlash == firstSlash + 1) {
            // v//vn
            vni = std::stoi(ref.substr(secondSlash + 1));
        } else {
            // v/vt/vn
            vti = std::stoi(ref.substr(firstSlash + 1, secondSlash - firstSlash -1));
            vni = std::stoi(ref.substr(secondSlash + 1));
        }
    }

    static std::array<float,3> fetchPosition(const std::vector<std::array<float,3>> &positions, int idx) {
        int i = resolveIndex(static_cast<int>(positions.size()), idx);
        return positions[i];
    }
    static std::array<float,3> fetchNormal(const std::vector<std::array<float,3>> &normals, int idx) {
        int i = resolveIndex(static_cast<int>(normals.size()), idx);
        return normals[i];
    }
    static std::array<float,2> fetchTexcoord(const std::vector<std::array<float,2>> &texcoords, int idx) {
        int i = resolveIndex(static_cast<int>(texcoords.size()), idx);
        return texcoords[i];
    }

    static int resolveIndex(int size, int idx) {
        // OBJ indices are 1-based. Negative indices count from the end.
        if (idx > 0) return idx - 1;
        if (idx < 0) return size + idx; // e.g. -1 refers to last element
        throw std::runtime_error("OBJ index 0 is invalid");
    }

    static void generateNormals(Mesh &mesh) {
        // Zero normals
        for (auto &v : mesh.vertices) { v.nx = v.ny = v.nz = 0.0f; }
        // Accumulate face normals
        for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
            uint32_t ia = mesh.indices[i+0];
            uint32_t ib = mesh.indices[i+1];
            uint32_t ic = mesh.indices[i+2];
            Vertex &A = mesh.vertices[ia];
            Vertex &B = mesh.vertices[ib];
            Vertex &C = mesh.vertices[ic];
            float ux = B.px - A.px; float uy = B.py - A.py; float uz = B.pz - A.pz;
            float vx = C.px - A.px; float vy = C.py - A.py; float vz = C.pz - A.pz;
            // normal = cross(u,v)
            float nx = uy * vz - uz * vy;
            float ny = uz * vx - ux * vz;
            float nz = ux * vy - uy * vx;
            // accumulate
            A.nx += nx; A.ny += ny; A.nz += nz;
            B.nx += nx; B.ny += ny; B.nz += nz;
            C.nx += nx; C.ny += ny; C.nz += nz;
        }
        // normalize
        for (auto &v : mesh.vertices) {
            float lx = v.nx, ly = v.ny, lz = v.nz;
            float len = std::sqrt(lx*lx + ly*ly + lz*lz);
            if (len > 1e-6f) { v.nx = lx/len; v.ny = ly/len; v.nz = lz/len; }
            else { v.nx = 0; v.ny = 0; v.nz = 1; }
        }
    }
};

} // namespace rmdl

// USAGE (example):
// #include "RMDLObjLoader.hpp"
// rmdl::RMDLObjLoader loader;
// rmdl::Mesh mesh = loader.loadObj("assets/model.obj");
// // If on macOS and using metal-cpp, define USE_METAL_CPP and call createMetalBuffers(mesh, device)


