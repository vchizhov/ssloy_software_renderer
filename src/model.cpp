#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.hpp"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail())
    {
        std::cerr << "Failed to load: " << filename << "\n";
        return;
    }
    std::string line;

    /* while we have not reached the end of the file */
    while (!in.eof()) {

        /* get one oine at a time */
        std::getline(in, line);
        /* make a stream from it */
        std::istringstream iss(line.c_str());

        char trash;

        /* if it is a line describing vertex coordinates */
        if (!line.compare(0, 2, "v ")) 
        {
            /* consume the 'v' symbol */
            iss >> trash;
            /* consume the coordinates */
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            /* store */
            verts_.push_back(v);
        
        } /* if it is a line describing vertex texture coordinates */
        else if (!line.compare (0, 3, "vt "))
        {
            /* consume symbol 'v' */
            iss >> trash;
            /* consume symbol 't' */
            iss >> trash;
            /* consume the coordinates */
            Vec3f tex;
            for (int i=0;i<3;i++) iss >> tex.raw[i];
            /* store */
            uvw_.push_back(tex);
        }
        else if (!line.compare (0, 3, "vn "))
        {
            /* consume symbol 'v' */
            iss >> trash;
            /* consume symbol 'n' */
            iss >> trash;
            /* consume the coordinates */
            Vec3f n;
            for (int i=0;i<3;i++) iss >> n.raw[i];
            /* store */
            normals_.push_back(n);
        }
        else if (!line.compare(0, 2, "f ")) {
            /* current face */
            std::vector<int> f;
            std::vector<int> f_t;
            std::vector<int> f_n;

            int idx;
            int t_idx, n_idx;

            /* consume 'f' symbol */
            iss >> trash;
            while (iss >> idx >> trash >> t_idx >> trash >> n_idx) {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
                f_t.push_back(t_idx-1);
                f_n.push_back(n_idx-1);
            }
            faces_.push_back(f);
            faces_tex_.push_back(f_t);
            faces_n_.push_back(f_n);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

const std::vector<int>& Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}
