#pragma once

template<typename Function>
void line_bresenham

     (const int        x0,        /* x coordindate of the first point */
      const int        y0,        /* y coordindate of the first point */
      const int        x1,        /* x coordindate of the second point */
      const int        y1,        /* y coordindate of the second point */
      Function&&       plot_fn)   /* function to set a pixel */

{
    /************************ Variable declarations ***************************/
    /* line start and endpoint */
    int sx;
    int sy;
    int ex;
    int ey;

    /* differences */
    int d2x;
    int d2y;
    int err;

    /* whether to swap X and Y */
    bool greater45;

    /* whether to swap start and end points */
    bool flipped_x;

    /* whether to increment or decrement */
    bool flipped_y;
    int incr;

    /* whether to stay with current Y */
    bool stay_y;


    /************************ Actual code ***************************/
    /* set up the start and end points */
    d2x = x0<=x1 ? x1-x0 : x0-x1;
    d2y = y0<=y1 ? y1-y0 : y0-y1;
    greater45 = d2x < d2y;
    flipped_x = greater45 ? y1<y0 : x1<x0;

    /* swap coords */
    sx = greater45 ? (flipped_x ? y1 : y0) : (flipped_x ? x1 : x0);
    sy = greater45 ? (flipped_x ? x1 : x0) : (flipped_x ? y1 : y0);
    ex = greater45 ? (flipped_x ? y0 : y1) : (flipped_x ? x0 : x1);
    ey = greater45 ? (flipped_x ? x0 : x1) : (flipped_x ? y0 : y1);

    /* increment */
    flipped_y = sy > ey;
    incr = flipped_y ? -1 : 1;

    /* compute differences */
    d2x = 2*(ex-sx);
    d2y = flipped_y ? 2*(sy-ey) : 2*(ey-sy);
    err = d2y - (ex-sx);

    int y = sy;
    for (int x=sx; x<=ex; ++x)
    {
        if (greater45)
            plot_fn(y,x);
        else
            plot_fn(x,y);
        stay_y = err <= 0;
        y = stay_y ? y : y + incr;
        err = err + d2y + (stay_y ? 0 : -d2x);
    }

    return;

}   /* line_bresenham */


/*
    Derivation:

    1)
    Let (sx,sy), (ex,ey) be the two points forming the line 
    to be rasterized. And let 0 <= (ey-sy) <= (ex-sx).

    2) Slope intercept line equation:
    tan(alpha) = (ey-sy)/(ex-sx) = (y-sy)/(x-sx) -> (let x!=sx)
    y = sy + (x-sx)*(ey-sy)/(ex-sx)

    3)
    At the k-th pixel starting from sx (x_k = sx + k) the y coordinate 
    should ideally be:
    y   = sy + (x_k-sx) * (ey-sy)/(ex-sx) 
        = sy + k * (ey-sy)/(ex-sx)

    Let the previously rasterized pixel have Y: y_{k-1}

    We want to find whether y is closer to y_{k-1} or y_{k-1} + 1.
    We can compare y to the mid-point y_{k-1}+0.5. If y <= y_{k-1} + 0.5 we 
    rasterize y_k = y_{k-1}, otherwise y_k = y_{k-1} + 1.
    We note that this inequality is equivalent to:
    e_k = y - (y_{k-1} + 0.5) <= 0

    4)
    From 3) we have a general formula which pixel to rasterize 
    at step k (given the previously rasterized pixel had Y: y_{k-1})
    if e_k <= 0 -> y_k = y_{k-1}
    else y_k = y_{k-1} + 1

    One could technically stop at this point. However, our goal would be to perform 
    all operations in integer arithmetic and only with addition/subtraction 
    at every step.

    We note that the computation of y involves a multiplication and division,
    while the computation of e_k involves an additional fractional value (0.5).
    Both of those can be optimized away, since our comparison is wrt 0:
    e_k <= 0

    We rewrite the expression to get an equivalent result that does not 
    require division or fractional values:

    e_k = y - (y_{k-1} + 0.5) <= 0 <->
    2 * y - 2*y_{k-1} - 1 <= 2 * 0 <->
    2*sy + 2*k*(ey-sy)/(ex-sx) - 2*y_{k-1} - 1 <= 0 <-> (using ex-sx>0)
    2*sy*(ex-sx) + 2*k*(ey-sy) - (ex-sx)*(2*y_{k-1}+1) <= (ex-sx) * 0
    
    We denote the new term:
    f_k = 2*sy*(ex-sx) + 2*k*(ey-sy) - (ex-sx)*(2*y_{k-1}+1) 

    The new equivalent condition for rasterizing the lower pixel is:
    f_k <= 0

    5)
    We can also get rid of the multiplication. This can be achieved by finding 
    an expression for f_{k+1} through f_k. To this end we compute the 2 cases 
    for f_{k+1}:

    if f_k <= 0 -> y_k = y_{k-1}
    f_{k+1} = 2*sy*(ex-sx) + 2*(k+1)*(ey-sy) - (ex-sx)*(2*y_{k-1}+1)
            = f_k + 2*(ey-sy)

    if f_k > 0 -> y_k = y_{k-1}+1
    f_{k+1} = 2*sy*(ex-sx) + 2*(k+1)*(ey-sy) - (ex-sx)*(2*(y_{k-1}+1)+1)
            = f_k + 2*(ey-sy) - 2*(ex-sx)

    
    Using the above we can construct f_{k+1} from f_k by adding 
    2*(ey-sy) or 2*(ey-sy) - 2*(ex-sx) respectively for f_k<=0 and f_k>0.

    All that remains to be done is finding the first term f_1.
    For f_1 we have that y_0 = sy, k = 1, using the expression for f_k from 4) we get:
    f_1 = 2*sy*(ex-sx) + 2*(ey-sy) - 2*sy*(ex-sx) - (ex-sx) = 2*(ey-sy)-(ex-sx)


    6)
    Clearly 2*(ey-sy) and 2*(ex-sx) can be precomputed.
    This yields an algorithm of the form:

    d2x = 2*(ex-sx)
    d2y = 2*(ey-sy)
    err = d2y - (ex-sx)
    y = sy
    for x = sx to ex
        if (err <= 0)
            err = err + d2y
        else
            y = y + 1
            err = err + d2y - d2x
        plot(x,y)

    However, in 1) we considered only the case where ex-sx>=ey-sy>=0.
    To properly draw a line for all octants, we need to consider all possible 
    cases.

    7)   
    Case: (ex-ey) >= (sy-ey) > 0, (sy>ey)
    In this case y needs to decrement, the new midpoint is: y_{k-1} - 0.5, 
    and the conditions <= and > are flipped.
    Decrementing is trivial by setting an increment variable:

    incr = ey>=sy ? 1 : -1,

    and using: 

    y = y + incr.

    Flipping the condition can be achieved by considering -f_k, 
    but we must also take into account the +0.5 term changing to -0.5.
    This cumulative results only flip the sign of d2y:

    err = -d2y - (ex-sx)

    and additions:

    err = err - d2y, err = err - d2y - d2x

    The above can be merged trivially with the previous by using d2y = 2*abs(ey-sy):

    d2x = 2*(ex-sx)
    incr = ey>=sy ? 1 : -1;
    d2y = 2*abs(ey-sy)
    err = d2y - (ex-sx)
    y = sy
    for x = sx to ex
        if (err <= 0)
            err = err + d2y
        else
            y = y + incr
            err = err + d2y - d2x
        plot(x,y)    

    8)
    Case: ex<sx, (sx-ex) >= (ey-sy) >=0 or (sx-ex) >= (sy-ey) > 0

    This case is trivial, since it reduces to the previous cases as 
    long as (sx,sy) is swapped with (ex,ey):

    if (sx>sy)
        swap(sx,ex)
        swap(sy,ey)

    9)
    Case: |ex-sx| < |ey-sy|
    This is the last case that must be handled which corresponds to lines 
    with an angle greater than 45 degrees wrt X axis.
    
    However, it results in the almost same code performed for x instead of 
    for y. To achieve this (sx,sy) and (ex,ey) can be swapped and then 
    plotting can be performed as plot(y,x) instead of plot(x,y).

    The final code handling all octants by combining all of the above is:

    sx = x0
    sy = y0
    ex = x1
    ey = y1
    bool greater45 = abs(x1-x0) < abs(y1-y0)
    if (greater45)
        swap(sx,sy)
        swap(ex,ey)
       
    if (sx>sy)
        swap(sx,ex)
        swap(sy,ey)

    incr = ey>=sy ? 1 : -1;
    d2x = 2*(ex-sx)
    d2y = 2*abs(ey-sy)
    err = d2y - (ex-sx)
    y = sy
    for x = sx to ex
        if (err <= 0)
            err = err + d2y
        else
            y = y + incr
            err = err + d2y - d2x
        if (greater45)
            plot(y,x)
        else
            plot(x,y)    

*/

/*
    Appendix:

    Line equations in 2D:
    |---------------------------------------------------------------------|
    |---------------------------------------------------------------------|
    |---------------------------------------------------------------------|
    * parametric form (convex linear combination of the start and end points):

    p(t) = (1-t) * v0 + t*v1 

    <=>

    x = (1-t)*x0 + t*x1 = x0 + t*(x1-x0)
    y = (1-t)*y0 + t*y0 = y0 + t*(y1-y0)

    for t in [0,1]

    - remark: extends to N dimensions, 
        results in N equations (dependent on t)

    |---------------------------------------------------------------------|
    |---------------------------------------------------------------------|
    |---------------------------------------------------------------------|
    * slope intercept form:

    tan(alpha) = (y1-y0) / (x1-x0) = (y-y0) / (x-x0)

    =>

    y = y0 + (x-x0) * (y1-y0) / (x1-x0) 

    for x in [x0,x1]

    - remark: fails for x1=x0
    - remark: extends to N dimensions, 
        results in N-1 equations (dependent on 
        the chosen oordinate)

    |---------------------------------------------------------------------|
    |---------------------------------------------------------------------|
    |---------------------------------------------------------------------|
    * canonical form:

    <n, p-p0> = 0, n != 0, (<a,b> is the dot product of a and b)
    <-> 1) p=p0 - trivial
    <-> 2) cos(angle(n, p-p0)) = 0 <-> angle(n, p-p0) = +- 90 degrees

    <=>

    nx * (x-x0) + ny * (y-y0) = 0

    Choices for the normal n up to a scaling factor:
    nx = -(y1-y0) * s, ny = (x1-x0) * s, s \in R / {0}

    -remark: extends to N dimensions, 
        results in N equations
    -remark if p is not on the line, gives the distance 
    along the normal direction from the line to the 
    point multiplied by the length of the normal:
    <p-p0,n> = ||p-p0|| * ||n|| * cos(p-p0,n)
*/