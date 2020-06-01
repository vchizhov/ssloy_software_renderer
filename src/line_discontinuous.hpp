#pragma once

template<typename Function>
void line_discontinuous_y

     (const int        x0,        /* x coordindate of the first point */
      const int        y0,        /* y coordindate of the first point */
      const int        x1,        /* x coordindate of the second point */
      const int        y1,        /* y coordindate of the second point */
      Function&&       plot_fn)   /* function to plot a pixel: auto(int,int) */

/*
    Rasterizes a line from (x0, y0) to (x1,y1) by allowing discontinuities 
    along the Y axis.

    The code is written to be as generic as possible and not rely on external 
    functions. This philosophy introduces some code bloat. External functions
    can be enabled disabled by setting: LINE_BRESENHAM_NO_FUNC to 0/1.

    The derivation of the algorithm at the end.
*/

{
    /************************ Variable declarations ***************************/
    /* line start and endpoint */
    int sx;
    int sy;
    int ex;
    int ey;

    /* differences */
    int dx;
    int dy;
    int delta;
    int rem;
    int err;
    int err_temp;
    int acc;

    /* whether to swap start and end points */
    bool flipped_x;

    /* whether to increment or decrement */
    bool flipped_y;
    int incr;
    


    /************************ Actual code ***************************/
    if (x1==x0) return;

    /* set up the start and end points */
    flipped_x = x1<x0;
    sx = flipped_x ? x1 : x0;
    sy = flipped_x ? y1 : y0;
    ex = flipped_x ? x0 : x1;
    ey = flipped_x ? y0 : y1;

    /* increment */
    flipped_y = sy > ey;
    incr = flipped_y ? -1 : 1;

    /* compute differences */
    dx = ex-sx;
    dy = ey-sy;
    delta = dy/dx;
    rem = flipped_y ? delta * dx - dy : dy - delta * dx;
    err = rem - dx;

    int y = sy;
    acc=0;
    for (int x=sx; x<=ex; ++x)
    {
        plot_fn(x,y);
        err_temp = err + rem;
        err = err_temp + (err_temp>=0 ? -dx : 0);
        acc = acc + delta + (err_temp>=0 ? incr : 0);
        y = sy + acc + (2*err+dx>0 ? incr : 0);
    }

    return;

}   /* line_discontinuous_y */