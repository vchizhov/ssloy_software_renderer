#pragma warning(disable : 4201)
#include <vector>
#include <cmath>
#include <iostream>
#include "tgaimage.hpp"
#include "model.hpp"
#include "geometry.hpp"
#include "line_bresenham.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

//void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
//    bool steep = false;
//    if (std::abs(x0-x1)<std::abs(y0-y1)) {
//        std::swap(x0, y0);
//        std::swap(x1, y1);
//        steep = true;
//    }
//    if (x0>x1) {
//        std::swap(x0, x1);
//        std::swap(y0, y1);
//    }
//
//    for (int x=x0; x<=x1; x++) {
//        float t = (x-x0)/(float)(x1-x0);
//        int y = y0*(1.-t) + y1*t;
//        if (steep) {
//            image.set(y, x, color);
//        } else {
//            image.set(x, y, color);
//        }
//    }
//}

TGAImage image(width, height, TGAImage::RGB);

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head/african_head.obj");
    }


    auto plot_fn = [](int x, int y) mutable { image.set(x,y,white); /*std::cout << "X,Y: " << x << ", " << y << "\n";*/};
    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);
            int x0 = (int)((v0.x+1.)*width/2.);
            int y0 = (int)((v0.y+1.)*height/2.);
            int x1 = (int)((v1.x+1.)*width/2.);
            int y1 = (int)((v1.y+1.)*height/2.);
            line_bresenham(x0, y0, x1, y1, plot_fn);
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
