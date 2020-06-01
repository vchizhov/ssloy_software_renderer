#pragma once

template<typename Function>
void triangle_barycentric

     (const int        x0,        /* x coordindate of the first vertex */
      const int        y0,        /* y coordindate of the first vertex */
      const int        x1,        /* x coordindate of the second vertex */
      const int        y1,        /* y coordindate of the second vertex */
      const int        x2,        /* x coordindate of the third vertex */
      const int        y2,        /* y coordindate of the third vertex */      
      Function&&       plot_fn)   /* function to set a pixel */

/*
    Rasterizes the inside of a triangle.
*/

{

    int ex[3];      /* triangle edges x coords as shown in the sketch below */
    int ey[3];      /* triangle edges y coords as shown in the sketch below */
    int area;          /* twice the triangle area */

    int x_min;      /* triangle aabb min x coord */
    int y_min;      /* triangle aabb min y coord */
    int x_max;      /* triangle aabb max x coord */
    int y_max;      /* triangle aabb max y coord */

    int a[3];       /* twice the areas of inner triangles */
    int a_row[3];   /* precomputed areas for each row */

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

            if (a[0] <= 0 && a[1] <= 0 && a[2] <= 0) // ((a[0] & a[1] & a[2] & -1) <= 0)//
                plot_fn(x,y);
        }

        a_row[0] -= ex[0];
        a_row[1] -= ex[1];
        a_row[2] -= ex[2];
    }

    return;

}   /* triangle_barycentric */
