#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "vboindexer.h"

typedef struct VertexInfo {
    float vx, vy, vz;
    float tx, ty;
    float nx, ny, nz;
} VertexInfo;

// typedef struct {
//     int v1, vt1, vn1;
//     int v2, vt2, vn2;
//     int v3, vt3, vn3;
// } VertexIndexInfo;

typedef struct {
    int vt = -1, vn = -1;
} VertexAttribute;

/**
 * @brief Helper class to extract vertices and normals values from .obj file
 */
class ObjExtractor {
 public:
    /**
     * @brief Construct a new Obj Extractor object
     */
    ObjExtractor() {}
    /**
     * @brief Destroy the Obj Extractor object
     */
    virtual ~ObjExtractor() {}
    /**
     * @brief Init with the obj file path
     * 
     * @param obj_path the path of the obj file to be extracted
     * @return true for read successfully
     * @return false for failure
     */
    bool init(std::string obj_path) {
        ifs_.open(obj_path);
        if (!ifs_.is_open()) {
            return false;
        }
        this->extract("v", vertice_, 3);
        this->extract("vt", textures_, 2);
        this->extract("vn", normals_, 3);
        this->extract_multi("f", vertice_idxs_, texture_idxs_, normal_idxs_);
        return true;
    }
    std::vector<VertexInfo> get_vertex_info() {
        std::vector<VertexInfo> ret;
        // buffer_.resize(vertice_.size());
        for (int i = 0; i < vertice_idxs_.size(); i++) {
            VertexInfo vert;
            for (auto vert_index : vertice_idxs_[i]) {
                // vert.vx = vertice_[i][0];
                // vert.vy = vertice_[i][1];
                // vert.vz = vertice_[i][2];
                int texture_index = buffer_[vert_index].vt;
                int normal_index = buffer_[vert_index].vn;
                // std::cout << "test: " << textures_.size() << ", " << vert_index << ", " <<texture_index << "\n";
                vert.tx = textures_[texture_index][0];
                vert.ty = textures_[texture_index][1];
                vert.nx = normals_[normal_index][0];
                vert.ny = normals_[normal_index][1];
                vert.nz = normals_[normal_index][2];
                ret.push_back(vert);
            }
        }
        return std::move(ret);
    }
    std::vector<unsigned int> get_vertex_index_info() {
        std::vector<unsigned int> ret;
        // ret = {0,1,2};
        for (int i = 0; i < vertice_idxs_.size(); i++) {
            ret.push_back(vertice_idxs_[i][0]);
            ret.push_back(vertice_idxs_[i][1]);
            ret.push_back(vertice_idxs_[i][2]);
        }
        return std::move(ret);
    }
    void get_infos(
        std::vector<unsigned int> &out_indices,
        std::vector<float> &out_vertices,
        std::vector<float> &out_uvs,
        std::vector<float> &out_normals) {
            // 
            out_indices.clear();
            out_vertices.clear();
            out_uvs.clear();
            out_normals.clear();
            // 
            std::vector<glm::vec3> _in_vertices;
            std::vector<glm::vec2> _in_uvs;
            std::vector<glm::vec3> _in_normals;
            std::vector<unsigned short> _out_indices;
            std::vector<glm::vec3> _out_vertices;
            std::vector<glm::vec2> _out_uvs;
            std::vector<glm::vec3> _out_normals;
            for (int idx = 0; idx < this->buffer_.size(); idx++) {
                auto vt = this->buffer_[idx].vt;
                auto vn = this->buffer_[idx].vn;
                _in_vertices.push_back(glm::vec3(this->vertice_[idx][0], this->vertice_[idx][1], this->vertice_[idx][2]));
                _in_uvs.push_back(glm::vec2(this->textures_[vt][0], this->textures_[vt][1]));
                _in_normals.push_back(glm::vec3(this->normals_[vn][0], this->normals_[vn][1], this->normals_[vn][2]));
            }
            indexVBO(_in_vertices, _in_uvs, _in_normals, _out_indices, _out_vertices, _out_uvs, _out_normals);
            for (auto val : _out_indices) {
                out_indices.push_back(val);
            }
            for (auto val : _out_vertices) {
                out_vertices.push_back(val[0]);
                out_vertices.push_back(val[1]);
                out_vertices.push_back(val[2]);
            }
            for (auto val : _out_uvs) {
                out_uvs.push_back(val[0]);
                out_uvs.push_back(val[1]);
            }
            for (auto val : _out_normals) {
                out_normals.push_back(val[0]);
                out_normals.push_back(val[1]);
                out_normals.push_back(val[2]);
            }
    }
	
    /**
     * @brief Get the vertices from the obj file
     * 
     * @param vertices the extracted vertices
     */
    void get_vertices(std::vector<float> &vertices) {
        vertices.clear();
        for (auto val : this->vertice_) {
            vertices.push_back(val[0]);
            vertices.push_back(val[1]);
            vertices.push_back(val[2]);
            // vertices.push_back(val[0] / 10.f);
            // vertices.push_back((val[1] - 158) / 15.f - 0.5);
            // vertices.push_back(val[2] / 7.f);
        }
    }
    /**
     * @brief Get the normals from the obj file
     * 
     * @param normals the extracted normals
     */
    void get_textures(std::vector<float> &textures) {
        textures.clear();
        for (auto val : this->textures_) {
            textures.push_back(val[0]);
            textures.push_back(val[1]);
            textures.push_back(val[2]);
        }
    }
    /**
     * @brief Get the normals from the obj file
     * 
     * @param normals the extracted normals
     */
    void get_normals(std::vector<float> &normals) {
        normals.clear();
        for (auto val : this->normals_) {
            normals.push_back(val[0]);
            normals.push_back(val[1]);
            normals.push_back(val[2]);
        }
    }
    /**
     * @brief Get the face vertex indices from the obj file
     * 
     * @param normals the extracted normals
     */
    void get_vertice_idxs(std::vector<unsigned int> &vertice_idxs) {
        vertice_idxs.clear();
        for (auto idx : this->vertice_idxs_) {
            vertice_idxs.push_back(idx[0]);
            vertice_idxs.push_back(idx[1]);
            vertice_idxs.push_back(idx[2]);
        }
    }
    /**
     * @brief Get the face textures indices from the obj file
     * 
     * @param normals the extracted normals
     */
    void get_texture_idxs(std::vector<unsigned int> &texture_idxs) {
        texture_idxs.clear();
        for (auto idx : this->texture_idxs_) {
            texture_idxs.push_back(idx[0]);
            texture_idxs.push_back(idx[1]);
            texture_idxs.push_back(idx[2]);
        }
    }
    /**
     * @brief Get the face normal indices from the obj file
     * 
     * @param normals the extracted normals
     */
    void get_normal_idxs(std::vector<unsigned int> &normal_idxs) {
        normal_idxs.clear();
        for (auto idx : this->normal_idxs_) {
            normal_idxs.push_back(idx[0]);
            normal_idxs.push_back(idx[1]);
            normal_idxs.push_back(idx[2]);
        }
    }

 private:
    /**
     * @brief Extract the values from obj file using specified indicator
     * 
     * @param indicator the indicator for the line read
     * @param vals the read arrays
     */
    void extract(std::string indicator, std::vector<std::vector<float>> &vals, int element_size) {
        std::string line;
        ifs_.clear();
        ifs_.seekg(0);
        vals.clear();
        while (std::getline(ifs_, line)) {
            std::istringstream ss(line);
            std::string tmp;
            ss >> tmp;
            if (indicator == tmp) {
                std::vector<float> vv;
                float val;
                for (int i = 0; i < element_size; i++) {
                    ss >> val;
                    vv.push_back(val);
                }
                vals.push_back(vv);
            }
        }
    }
    void extract_multi(std::string indicator,
            std::vector<std::vector<int>> &vertices,
            std::vector<std::vector<int>> &textures,
            std::vector<std::vector<int>> &normals) {
        std::string line;
        ifs_.clear();
        ifs_.seekg(0);
        vertices.clear();
        textures.clear();
        normals.clear();
        buffer_.resize(this->vertice_.size());
        while (std::getline(ifs_, line)) {
            std::istringstream ss(line);
            std::string tmp;
            ss >> tmp;
            if (indicator == tmp) {
                std::string f1, f2, f3;
                ss >> f1 >> f2 >> f3;
                auto split = [](char delim, std::string str){
                    std::vector<int> idx;
                    std::string v;
                    for (int i = 0; i < str.length(); i++) {
                        if (str[i] == delim) {
                            idx.push_back(std::stoi(v));
                            v = "";
                        } else {
                            v.push_back(str[i]);
                        }
                    }
                    if (!v.empty()) idx.push_back(std::stoi(v));
                    return idx;
                };
                std::vector<int> idxs1 = split('/', f1);
                std::vector<int> idxs2 = split('/', f2);
                std::vector<int> idxs3 = split('/', f3);
                int ele_size = idxs1.size();
                if (ele_size > 0) {
                    vertices.push_back({idxs1[0]-1, idxs2[0]-1, idxs3[0]-1});
                }
                if (ele_size > 1) {
                    textures.push_back({idxs1[1]-1, idxs2[1]-1, idxs3[1]-1});
                    buffer_[idxs1[0]-1].vt = idxs1[1]-1;
                    buffer_[idxs2[0]-1].vt = idxs2[1]-1;
                    buffer_[idxs3[0]-1].vt = idxs3[1]-1;
                }
                if (ele_size > 2) {
                    normals.push_back({idxs1[2]-1, idxs2[2]-1, idxs3[2]-1});
                    buffer_[idxs1[0]-1].vn = idxs1[2]-1;
                    buffer_[idxs2[0]-1].vn = idxs2[2]-1;
                    buffer_[idxs3[0]-1].vn = idxs3[2]-1;
                }
            }
        }
    }

 private:
    std::ifstream ifs_;
    std::vector<std::vector<float>> vertice_;
    std::vector<std::vector<float>> textures_;
    std::vector<std::vector<float>> normals_;
    std::vector<std::vector<int>> vertice_idxs_;
    std::vector<std::vector<int>> texture_idxs_;
    std::vector<std::vector<int>> normal_idxs_;
    
    std::vector<VertexAttribute> buffer_;
};
