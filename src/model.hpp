#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.hpp"

class Model {
private:
	std::vector<Vec3f> verts_;
    std::vector<Vec3f> uvw_;
    std::vector<Vec3f> normals_;
	std::vector<std::vector<int> > faces_;
    std::vector<std::vector<int>> faces_tex_;
    std::vector<std::vector<int>> faces_n_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
    Vec3f uvw(int i) { return uvw_[i]; }
    Vec3f normal(int i) { return normals_[i]; }
	const std::vector<int>& face(int idx);
	const std::vector<int>& face_tex(int idx) { return faces_tex_[idx]; }
    const std::vector<int>& face_n(int idx) { return faces_n_[idx]; }
};

#endif //__MODEL_H__