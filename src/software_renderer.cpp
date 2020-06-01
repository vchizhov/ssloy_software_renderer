#pragma warning(disable : 4201)
#include <vector>
#include <cmath>
#include <iostream>
#include "tgaimage.hpp"
#include "model.hpp"
#include "geometry.hpp"
#include "line_bresenham.hpp"
#include "line_discontinuous.hpp"
#include "triangle_barycentric.hpp"
#include "triangle_depth.hpp"
#include "triangle_depth_texture.hpp"
#include "triangle_depth_texture_gouraud.hpp"
#include "matrix.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

/*
    Wireframe rendering using Bresenham's line algoright.
*/
void lesson1(int argc, char** argv)
{
    /* load model */
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head/african_head.obj");
    }

    /* output image */
    TGAImage image(width, height, TGAImage::RGB);
    auto plot_fn = [&image](int x, int y) { image.set(x,y,white);};

    /* iterate over all model faces */
    for (int i=0; i<model->nfaces(); i++)
    {
        /* extract the indices of the vertices' making up the face */
        const std::vector<int>& face = model->face(i);
        const int n = (int)face.size();
        /* iterate over all edges of the face (assume closed polygon) */
        for (int j=0; j<n; ++j) 
        {
            /* tail (v0) and head (v1) of the face */
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%n]);

            /* 
                1) Throw away Z coord (orthographic projection on XY plane)
                2) Viewport mapping: map [-1,1]^2 to [0,w] x [0,h]
            */
            int x0 = (int)((v0.x+1.)*width/2.);
            int y0 = (int)((v0.y+1.)*height/2.);
            int x1 = (int)((v1.x+1.)*width/2.);
            int y1 = (int)((v1.y+1.)*height/2.);

            /* draw projected edge */
            line_discontinuous_y(x0, y0, x1, y1, plot_fn);
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
}

/*
    Random colors render using barycentric triangle fill.
*/
void lesson2_05(int argc, char** argv)
{
    /* load model */
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head/african_head.obj");
    }

    /* output image */
    TGAImage image(width, height, TGAImage::RGB);

    /* iterate over all model faces */
    for (int i=0; i<model->nfaces(); i++)
    {
        /* extract the indices of the vertices' making up the face */
        const std::vector<int>& face = model->face(i);
        int x[3];
        int y[3];
        /* extract the first 3 vertices of a face and form a triangle */
        for (int j=0; j<3; ++j) 
        {
            Vec3f v = model->vert(face[j]);
            /* project vertex orthographically, and remap from [-1,1]^2 to [0,width]x[0,height] */
            x[j] = (int)(0.5f * (v.x + 1.0f) * width);
            y[j] = (int)(0.5f * (v.y + 1.0f) * height);
        }

        /* draw triangle with random colors */
        TGAColor rand_color = TGAColor(rand() % 256, rand() % 256, rand() % 256, 255);
        auto plot_fn = [&image, &rand_color](int x, int y) { image.set(x, y, rand_color);};
        triangle_barycentric (x[0], y[0], x[1], y[1], x[2], y[2], plot_fn);
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
}

/*
    Diffuse light render using barycentric triangle fill.
*/
void lesson2(int argc, char** argv)
{
    /* load model */
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head/african_head.obj");
    }

    /* output image */
    TGAImage image(width, height, TGAImage::RGB);

    Vec3f light_dir = Vec3f(0.0f, 0.0f, 1.0f);

    /* iterate over all model faces */
    for (int i=0; i<model->nfaces(); i++)
    {
        /* extract the indices of the vertices' making up the face */
        const std::vector<int>& face = model->face(i);

        Vec3f v[3]; /* world vertices' coordinates */

        /* projected vertices' coordinates */
        int x[3];
        int y[3];

        /* extract the first 3 vertices of a face and form a triangle */
        for (int j=0; j<3; ++j) 
        {
            v[j] = model->vert(face[j]);
            /* project vertex orthographically, and remap from [-1,1]^2 to [0,width]x[0,height] */
            x[j] = (int)(0.5f * (v[j].x + 1.0f) * width);
            y[j] = (int)(0.5f * (v[j].y + 1.0f) * height);
        }

        Vec3f face_normal = normalize(cross(v[1]-v[0], v[2]-v[0]));
        float cosine = -dot(face_normal, light_dir);
        unsigned char r = (unsigned char)clamp(cosine*256.0f, 0.0f, 255.0f);
        unsigned char g = r;
        unsigned char b = r;
        /* draw triangle with random colors */
        TGAColor rand_color = TGAColor(r, g, b, 255);
        auto plot_fn = [&image, &rand_color](int x, int y) { image.set(x, y, rand_color);};
        if (cosine>0)
            triangle_barycentric (x[0], y[0], x[1], y[1], x[2], y[2], plot_fn);
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
}

/*
    Depth render.
*/
void lesson3_05(int argc, char** argv)
{
    /* load model */
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head/african_head.obj");
    }

    /* output image */
    TGAImage image(width, height, TGAImage::RGB);

    /* Z-Buffer */
    float* zbuffer = new float[width*height];
    for (int i=0; i<width*height; ++i)
    {
        zbuffer[i] = std::numeric_limits<float>::infinity();
        image.set(i%width, i/width, TGAColor(0,0,0,255));
    }
    

    /* iterate over all model faces */
    for (int i=0; i<model->nfaces(); i++)
    {
        /* extract the indices of the vertices' making up the face */
        const std::vector<int>& face = model->face(i);

        /* projected vertices' coordinates */
        int x[3];
        int y[3];

        /* vertices' world z coord flipped and offset by 1 */
        float z[3];

        /* extract the first 3 vertices of a face and form a triangle */
        for (int j=0; j<3; ++j) 
        {
            Vec3f v = model->vert(face[j]);
            /* project vertex orthographically, and remap from [-1,1]^2 to [0,width]x[0,height] */
            x[j] = (int)(0.5f * (v.x + 1.0f) * width);
            y[j] = (int)(0.5f * (v.y + 1.0f) * height);
            z[j] = -v.z+1.0f;  /* flip and offset face */
        }

        auto plot_fn = [&image, &zbuffer](int x, int y, float z) 
        { 
            /* if the pixel is closer - rasterize it */
            int idx = x + y*width;
            if (zbuffer[idx] > z)
            {
                /* if the pixel is closer, update the z buffer */
                zbuffer[idx] = z;
                /* rasterize depth nicely */
                unsigned char g = (unsigned char)clamp(1/(5.0f*z) * 256.0f, 0.0f, 255.0f); 
                image.set(x,y,TGAColor(g,g,g,255));
            }
        };
        triangle_barycentric_depth (x[0], y[0], z[0], x[1], y[1], z[1], x[2], y[2], z[2], plot_fn);

    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    delete[] zbuffer;
}

/*
    Depth + diffuse render.

    Added gamma correction to match the image from the tutorial.
*/
void lesson3(int argc, char** argv)
{
    /* load model */
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head/african_head.obj");
    }

    /* output image */
    TGAImage image(width, height, TGAImage::RGB);

    /* Z-Buffer */
    float* zbuffer = new float[width*height];
    for (int i=0; i<width*height; ++i)
    {
        zbuffer[i] = std::numeric_limits<float>::infinity();
        image.set(i%width, i/width, TGAColor(0,0,0,255));
    }
    
    Vec3f light_dir = Vec3f(0,0,-1);

    /* iterate over all model faces */
    for (int i=0; i<model->nfaces(); i++)
    {
        /* extract the indices of the vertices' making up the face */
        const std::vector<int>& face = model->face(i);

        /* projected vertices' coordinates */
        int x[3];
        int y[3];

        /* vertices' world z coord flipped and offset by 1 */
        float z[3];

        /* vertices world coordinates */
        Vec3f v[3];

        /* extract the first 3 vertices of a face and form a triangle */
        for (int j=0; j<3; ++j) 
        {
            v[j] = model->vert(face[j]);
            /* project vertex orthographically, and remap from [-1,1]^2 to [0,width]x[0,height] */
            x[j] = (int)(0.5f * (v[j].x + 1.0f) * width);
            y[j] = (int)(0.5f * (v[j].y + 1.0f) * height);
            z[j] = -v[j].z+1.0f;  /* flip and offset face - necessary because of how the model is oriented*/
        }

        Vec3f face_normal = normalize(cross(v[1]-v[0], v[2]-v[0]));
        float cosine = clamp(-dot(face_normal, light_dir), 0.0f, 1.0f);
        /* gamma correct */
        unsigned char r = (unsigned char)clamp(powf(cosine,2.2f)*256.0f, 0.0f, 255.0f);

        auto plot_fn = [&image, &zbuffer, &r](int x, int y, float z) 
        { 
            /* if the pixel is closer - rasterize it */
            int idx = x + y*width;
            if (zbuffer[idx] > z)
            {
                /* if the pixel is closer, update the z buffer */
                zbuffer[idx] = z;
                image.set(x,y,TGAColor(r,r,r,255));
            }
        };
        triangle_barycentric_depth (x[0], y[0], z[0], x[1], y[1], z[1], x[2], y[2], z[2], plot_fn);

    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    delete[] zbuffer;
}

/*
    Diffuse + textures render.
*/
void lesson3_hw(int argc, char** argv)
{
    TGAImage texture_diffuse;
    /* load model */
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head/african_head.obj");
        texture_diffuse.read_tga_file ("../obj/african_head/african_head_diffuse.tga");
    }

    const int tw = texture_diffuse.get_width();
    const int th = texture_diffuse.get_height();

    /* convert texture to float */
    texture_diffuse.flip_vertically();
    Vec3f* tex_diff = new Vec3f[tw*th];
    for (int i=0; i<tw*th; ++i)
    {
        TGAColor col;
        Vec3f col_float;
        col = texture_diffuse.get(i%tw, i/tw);
        col_float.x = powf(col.r/255.0f, 1.0f/2.2f);
        col_float.y = powf(col.g/255.0f, 1.0f/2.2f);
        col_float.z = powf(col.b/255.0f, 1.0f/2.2f);
        tex_diff[i] = col_float;
    }

    /* output image */
    TGAImage image(width, height, TGAImage::RGB);

    /* Z-Buffer */
    float* zbuffer = new float[width*height];
    for (int i=0; i<width*height; ++i)
    {
        zbuffer[i] = std::numeric_limits<float>::infinity();
        image.set(i%width, i/width, TGAColor(0,0,0,255));
    }
    
    Vec3f light_dir = Vec3f(0,0,-1);

    /* iterate over all model faces */
    for (int i=0; i<model->nfaces(); i++)
    {
        /* extract the indices of the vertices' making up the face */
        const std::vector<int>& face = model->face(i);
        
        /* extract the indices of the vertices' tex coords making up the face */
        const std::vector<int>& face_tex = model->face_tex(i);

        /* projected vertices' coordinates */
        int x[3];
        int y[3];

        /* vertices' world z coord flipped and offset by 1 */
        float z[3];

        /* vertices' tex coords */
        float s[3];
        float t[3];

        /* vertices world coordinates */
        Vec3f v[3];

        /* extract the first 3 vertices of a face and form a triangle */
        for (int j=0; j<3; ++j) 
        {
            v[j] = model->vert(face[j]);
            /* project vertex orthographically, and remap from [-1,1]^2 to [0,width]x[0,height] */
            x[j] = (int)(0.5f * (v[j].x + 1.0f) * width);
            y[j] = (int)(0.5f * (v[j].y + 1.0f) * height);

            z[j] = -v[j].z+1.0f;  /* flip and offset face - necessary because of how the model is oriented*/

            Vec3f uvw = model->uvw(face_tex[j]);
            s[j] = uvw.x; //clamp((int)(uvw.x * tw), 0, tw-1);
            t[j] = uvw.y; //clamp((int)(uvw.y * th), 0, th-1);
        }

        Vec3f face_normal = normalize(cross(v[1]-v[0], v[2]-v[0]));
        float cosine = clamp(-dot(face_normal, light_dir), 0.0f, 1.0f);
        /* gamma correct */
        auto plot_fn = [&image, &zbuffer, &cosine, &tex_diff, &tw, &th](int x, int y, float z, float u, float v) 
        { 

            /* if the pixel is closer - rasterize it */
            int idx = x + y*width;
            if (zbuffer[idx] > z)
            {
                /* if the pixel is closer, update the z buffer */
                zbuffer[idx] = z;

                /* get texture coordinates */
                int tx = clamp((int)roundf(u * tw), 0, tw-1);
                int ty = clamp((int)roundf(v * th), 0, th-1);
                /* sample texture */
                Vec3f col_float = cosine * tex_diff[tx+ty*tw];

                /* gamma correct, quantize and write */
                TGAColor col;
                col.r = (unsigned char)clamp(256.0f*powf(col_float.x,2.2f), 0.0f, 255.0f);
                col.g = (unsigned char)clamp(256.0f*powf(col_float.y,2.2f), 0.0f, 255.0f);
                col.b = (unsigned char)clamp(256.0f*powf(col_float.z,2.2f), 0.0f, 255.0f);
                col.a = 255;
                image.set(x,y,col);
            }
        };
        triangle_barycentric_depth_texture (x[0], y[0], z[0], s[0], t[0], 
                                        x[1], y[1], z[1], s[1], t[1], 
                                        x[2], y[2], z[2], s[2], t[2], plot_fn);

    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    delete[] zbuffer;
    delete[] tex_diff;
}

/*
    Perspective projection.
*/
void lesson4(int argc, char** argv)
{
    TGAImage texture_diffuse;
    /* load model */
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head/african_head.obj");
        texture_diffuse.read_tga_file ("../obj/african_head/african_head_diffuse.tga");
    }

    const int tw = texture_diffuse.get_width();
    const int th = texture_diffuse.get_height();

    /* convert texture to float */
    texture_diffuse.flip_vertically();
    Vec3f* tex_diff = new Vec3f[tw*th];
    for (int i=0; i<tw*th; ++i)
    {
        TGAColor col;
        Vec3f col_float;
        col = texture_diffuse.get(i%tw, i/tw);
        col_float.x = powf(col.r/255.0f, 1.0f/2.2f);
        col_float.y = powf(col.g/255.0f, 1.0f/2.2f);
        col_float.z = powf(col.b/255.0f, 1.0f/2.2f);
        tex_diff[i] = col_float;
    }

    /* output image */
    TGAImage image(width, height, TGAImage::RGB);

    /* Z-Buffer */
    float* zbuffer = new float[width*height];
    for (int i=0; i<width*height; ++i)
    {
        zbuffer[i] = std::numeric_limits<float>::infinity();
        image.set(i%width, i/width, TGAColor(0,0,0,255));
    }
    
    Vec3f light_dir = Vec3f(0,0,-1);

    /* Consider camera to be at (0,0,0) and film to be at z=1 */
    float film_z = 1.0f;
    
    /* x/z = x'/film_z -> x' = film_z * x/z */

    /* iterate over all model faces */
    for (int i=0; i<model->nfaces(); i++)
    {
        /* extract the indices of the vertices' making up the face */
        const std::vector<int>& face = model->face(i);
        
        /* extract the indices of the vertices' tex coords making up the face */
        const std::vector<int>& face_tex = model->face_tex(i);

        /* projected vertices' coordinates */
        int x[3];
        int y[3];

        /* vertices' world z coord flipped and offset by 3 */
        float z[3];

        /* vertices' tex coords */
        float s[3];
        float t[3];

        /* vertices world coordinates */
        Vec3f v[3];

        /* extract the first 3 vertices of a face and form a triangle */
        for (int j=0; j<3; ++j) 
        {
            v[j] = model->vert(face[j]);
            v[j].z = -v[j].z + 1.5f; /* flip and translate vertex */

            /* perspective projection */
            v[j].x = film_z * v[j].x / v[j].z;
            v[j].y = film_z * v[j].y / v[j].z;

            /* project vertex orthographically, and remap from [-1,1]^2 to [0,width]x[0,height] */
            x[j] = clamp((int)(0.5f * (v[j].x + 1.0f) * width), 0, width-1);
            y[j] = clamp((int)(0.5f * (v[j].y + 1.0f) * height), 0, height-1);
        
            /* keep z for z-buffer */
            z[j] = v[j].z;

            Vec3f uvw = model->uvw(face_tex[j]);
            s[j] = uvw.x; //clamp((int)(uvw.x * tw), 0, tw-1);
            t[j] = uvw.y; //clamp((int)(uvw.y * th), 0, th-1);
        }

        Vec3f face_normal = normalize(cross(v[1]-v[0], v[2]-v[0]));
        float cosine = clamp(-dot(face_normal, light_dir), 0.0f, 1.0f);
        /* gamma correct */
        auto plot_fn = [&image, &zbuffer, &cosine, &tex_diff, &tw, &th](int x, int y, float z, float u, float v) 
        { 

            /* if the pixel is closer - rasterize it */
            int idx = x + y*width;
            if (zbuffer[idx] > z)
            {
                /* if the pixel is closer, update the z buffer */
                zbuffer[idx] = z;

                /* get texture coordinates */
                int tx = clamp((int)roundf(u * tw), 0, tw-1);
                int ty = clamp((int)roundf(v * th), 0, th-1);
                /* sample texture */
                Vec3f col_float = cosine * tex_diff[tx+ty*tw];

                /* gamma correct, quantize and write */
                TGAColor col;
                col.r = (unsigned char)clamp(256.0f*powf(col_float.x,2.2f), 0.0f, 255.0f);
                col.g = (unsigned char)clamp(256.0f*powf(col_float.y,2.2f), 0.0f, 255.0f);
                col.b = (unsigned char)clamp(256.0f*powf(col_float.z,2.2f), 0.0f, 255.0f);
                col.a = 255;
                image.set(x,y,col);
            }
        };
        triangle_barycentric_depth_texture (x[0], y[0], z[0], s[0], t[0], 
                                        x[1], y[1], z[1], s[1], t[1], 
                                        x[2], y[2], z[2], s[2], t[2], plot_fn);

    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    delete[] zbuffer;
    delete[] tex_diff;
}

/*
    Transformations. Gouraud shading.
*/
void lesson5(int argc, char** argv)
{
    TGAImage texture_diffuse;
    /* load model */
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head/african_head.obj");
        texture_diffuse.read_tga_file ("../obj/african_head/african_head_diffuse.tga");
    }

    const int tw = texture_diffuse.get_width();
    const int th = texture_diffuse.get_height();

    /* convert texture to float */
    texture_diffuse.flip_vertically();
    Vec3f* tex_diff = new Vec3f[tw*th];
    for (int i=0; i<tw*th; ++i)
    {
        TGAColor col;
        Vec3f col_float;
        col = texture_diffuse.get(i%tw, i/tw);
        col_float.x = powf(col.r/255.0f, 1.0f/2.2f);
        col_float.y = powf(col.g/255.0f, 1.0f/2.2f);
        col_float.z = powf(col.b/255.0f, 1.0f/2.2f);
        tex_diff[i] = col_float;
    }

    /* output image */
    TGAImage image(width, height, TGAImage::RGB);

    /* Z-Buffer */
    float* zbuffer = new float[width*height];
    for (int i=0; i<width*height; ++i)
    {
        zbuffer[i] = std::numeric_limits<float>::infinity();
        image.set(i%width, i/width, TGAColor(0,0,0,255));
    }
    
    Vec3f light_dir = Vec3f(0,0,-1);

    /* Consider camera to be at (0,0,0) and film to be at z=1 */
    float film_z = 1.0f;
    
    /* x/z = x'/film_z -> x' = film_z * x/z */

    mat4 camera_mat = look_at(vec3(1.0f,0.5f,0.f), vec3(0.0f,0.0f,1.5f), vec3(0,1,0));

    /* iterate over all model faces */
    for (int i=0; i<model->nfaces(); i++)
    {
        /* extract the indices of the vertices' making up the face */
        const std::vector<int>& face = model->face(i);
        
        /* extract the indices of the vertices' tex coords making up the face */
        const std::vector<int>& face_tex = model->face_tex(i);

        /* extract the indices of the vertices' normals */
        const std::vector<int>& face_n = model->face_n(i);

        /* projected vertices' coordinates */
        int x[3];
        int y[3];

        /* vertices' world z coord flipped and offset by 3 */
        float z[3];

        /* vertices' tex coords */
        float s[3];
        float t[3];

        /* vertices world coordinates */
        Vec3f v[3];

        /* normal world coordinates */
        Vec3f n;

        /* cosines */
        float cosine[3];

        /* extract the first 3 vertices of a face and form a triangle */
        for (int j=0; j<3; ++j) 
        {
            v[j] = model->vert(face[j]);
            v[j].z = -v[j].z + 1.5f; /* flip and translate vertex */

            /* extend to homogeneous and translate*/
            vec4 v_tform = vec4(v[j].x, v[j].y, v[j].z, 1.0f);

            /* multiplication from the left is equiv to mult by transpose -> equiv to inverse for this simple matrix */
            v_tform = mult(camera_mat, v_tform);

            /* perspective projection */
            v[j].x = film_z * v_tform[0] / v[j].z;
            v[j].y = film_z * v_tform[1] / v[j].z;
            v[j].z = v_tform[2]; 

            /* project vertex orthographically, and remap from [-1,1]^2 to [0,width]x[0,height] */
            x[j] = clamp((int)(0.5f * (v[j].x + 1.0f) * width), 0, width-1);
            y[j] = clamp((int)(0.5f * (v[j].y + 1.0f) * height), 0, height-1);
        
            /* keep z for z-buffer */
            z[j] = v[j].z;

            /* texture coordinates */
            Vec3f uvw = model->uvw(face_tex[j]);
            s[j] = uvw.x; //clamp((int)(uvw.x * tw), 0, tw-1);
            t[j] = uvw.y; //clamp((int)(uvw.y * th), 0, th-1);

            /* normal */
            n = model->normal(face_n[j]);
            cosine[j] = clamp(-dot(n, light_dir), 0.0f, 1.0f);
        }

        /* gamma correct */
        auto plot_fn = [&image, &zbuffer, &tex_diff, &tw, &th](int x, int y, float z, float u, float v, float cosine) 
        { 

            /* if the pixel is closer - rasterize it */
            int idx = x + y*width;
            if (zbuffer[idx] > z)
            {
                /* if the pixel is closer, update the z buffer */
                zbuffer[idx] = z;

                /* get texture coordinates */
                int tx = clamp((int)roundf(u * tw), 0, tw-1);
                int ty = clamp((int)roundf(v * th), 0, th-1);
                /* sample texture */
                Vec3f col_float = cosine * tex_diff[tx+ty*tw];

                /* gamma correct, quantize and write */
                TGAColor col;
                col.r = (unsigned char)clamp(256.0f*powf(col_float.x,2.2f), 0.0f, 255.0f);
                col.g = (unsigned char)clamp(256.0f*powf(col_float.y,2.2f), 0.0f, 255.0f);
                col.b = (unsigned char)clamp(256.0f*powf(col_float.z,2.2f), 0.0f, 255.0f);
                col.a = 255;
                image.set(x,y,col);
            }
        };
        triangle_barycentric_depth_texture_gouraud (x[0], y[0], z[0], s[0], t[0], cosine[0],
                                        x[1], y[1], z[1], s[1], t[1], cosine[1],
                                        x[2], y[2], z[2], s[2], t[2], cosine[2], plot_fn);

    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    delete[] zbuffer;
    delete[] tex_diff;
}

int main(int argc, char** argv) 
{
    lesson5(argc, argv);
    return 0;
}
