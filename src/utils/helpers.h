#pragma once

#include <vector>
#include <ostream>
#include "glm/glm.hpp"

static std::vector<float> glm_mat4_to_std_vec(glm::mat4 mat, bool row_major) {
    std::vector<float> ret;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ret.push_back(mat[i][j]);
        }
    }
    return ret;
}

static std::ostream& operator<< (std::ostream& os, glm::mat4& mat) {
    os << "Print Mat:\n";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            os << mat[i][j] << ", ";
        }
        os << "\n";
    }
    return os;
}