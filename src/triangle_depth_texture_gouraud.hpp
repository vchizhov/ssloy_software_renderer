#pragma once

template<typename Function>
void triangle_barycentric_depth_texture_gouraud

     (const int        x0,        /* x coordinate of the first vertex */
      const int        y0,        /* y coordinate of the first vertex */
      const float      z0,        /* z coordinate of the first vertex */
      const float      u0,        /* u texture coordinate of the first vertex */
      const float      v0,        /* v texture coordinate of the first vertex */
      const float      c0,        /* float to be inteprolated */
      const int        x1,        /* x coordinate of the second vertex */
      const int        y1,        /* y coordinate of the second vertex */
      const float      z1,        /* z coordinate of the second vertex */
      const float      u1,        /* u texture coordinate of the second vertex */
      const float      v1,        /* v texture coordinate of the second vertex */
      const float      c1,        /* float to be inteprolated */
      const int        x2,        /* x coordinate of the third vertex */
      const int        y2,        /* y coordinate of the third vertex */
      const float      z2,        /* z coordinate of the third vertex */     
      const float      u2,        /* u texture coordinate of the third vertex */
      const float      v2,        /* v texture coordinate of the third vertex */ 
      const float      c2,        /* float to be inteprolated */
      Function&&       plot_fn)   /* function to set a pixel */

/*
    Intrepolates depth and texture information.
*/

{

    int ex[3];      /* triangle edges x coords as shown in the sketch below */
    int ey[3];      /* triangle edges y coords as shown in the sketch below */
    int area;       /* twice the triangle area */

    int x_min;      /* triangle aabb min x coord */
    int y_min;      /* triangle aabb min y coord */
    int x_max;      /* triangle aabb max x coord */
    int y_max;      /* triangle aabb max y coord */

    int a[3];       /* twice the areas of inner triangles */
    int a_row[3];   /* precomputed areas for each row */

    float inv_area; /* inv_area = 1.0f/area */
    float w[3];     /* weights */

    /*  
        counter clockwise edges for X right, Y up
        
        v2                        +Y
        |\                        ^  
        | \                      /|\ 
        |  \                      |
     e1 |   \ e0                  |
        |    \                    |
        |     \                   |             
        |_ _ _ \                  |_ _ _ _ _ _ _\  +X 
       v0  e2   v1                              /   
                                                
    
    */
    ex[0] = x2-x1;
    ey[0] = y2-y1;
    ex[1] = x0-x2;
    ey[1] = y0-y2;
    ex[2] = x1-x0;
    ey[2] = y1-y0;

    /* twice the signed area - negative for the above coord system and a ccw tri */
    area = ex[2] * ey[1] - ex[1] * ey[2];

    if (area==0) return;
    inv_area = 1.0f/(float)area;

    /* find bounding box */
    x_min = (x0 <= x1 && x0 <= x2) ? x0 : (x1 <= x0 && x1 <= x2) ? x1 : x2;
    y_min = (y0 <= y1 && y0 <= y2) ? y0 : (y1 <= y0 && y1 <= y2) ? y1 : y2;
    x_max = (x0 >= x1 && x0 >= x2) ? x0 : (x1 >= x0 && x1 >= x2) ? x1 : x2;
    y_max = (y0 >= y1 && y0 >= y2) ? y0 : (y1 >= y0 && y1 >= y2) ? y1 : y2;    
    
    a_row[0] = (x_min-x1) * ey[0] - (y_min-y1) * ex[0];
    a_row[1] = (x_min-x2) * ey[1] - (y_min-y2) * ex[1];
    a_row[2] = (x_min-x0) * ey[2] - (y_min-y0) * ex[2];

    for (int y=y_min; y<=y_max; ++y)
    {
        a[0] = a_row[0];
        a[1] = a_row[1];
        a[2] = a_row[2];

        for (int x=x_min; x<=x_max; ++x)
        {
            /*
                a[0] = (x-x1) * ey[0] - (y-y1) * ex[0];
                a[1] = (x-x2) * ey[1] - (y-y2) * ex[1];
                a[2] = (x-x0) * ey[2] - (y-y0) * ex[2];
            */

            a[0] += ey[0];
            a[1] += ey[1];
            a[2] += ey[2];

            w[0] = a[0] * inv_area;
            w[1] = a[1] * inv_area;
            w[2] = a[2] * inv_area;
            if (a[0] <= 0 && a[1] <= 0 && a[2] <= 0) // ((a[0] & a[1] & a[2] & -1) <= 0)//
            {
                float z = z0 * w[0] + z1 * w[1] + z2 * w[2];
                float u = u0 * w[0] + u1 * w[1] + u2 * w[2];
                float v = v0 * w[0] + v1 * w[1] + v2 * w[2];
                float c = c0 * w[0] + c1 * w[1] + c2 * w[2];
                plot_fn(x, y, z, u, v, c);
            }
        }

        a_row[0] -= ex[0];
        a_row[1] -= ex[1];
        a_row[2] -= ex[2];
    }

    return;

}   /* triangle_barycentric_depth_texture_gouraud */
