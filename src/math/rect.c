#include <SDL2/SDL.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "rect.h"

Rect rect(float x, float y, float w, float h)
{
    const Rect result = {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };

    return result;
}

Rect rect_from_vecs(Point position, Vec size)
{
    return rect(position.x, position.y, size.x, size.y);
}

Rect rect_from_sdl(const SDL_Rect *rect)
{
    const Rect result = {
        .x = (float) rect->x,
        .y = (float) rect->y,
        .w = (float) rect->w,
        .h = (float) rect->h
    };

    return result;
}

Rect rects_overlap_area(Rect rect1, Rect rect2)
{
    float x1 = fmaxf(rect1.x, rect2.x);
    float y1 = fmaxf(rect1.y, rect2.y);
    float x2 = fminf(rect1.x + rect1.w, rect2.x + rect2.w);
    float y2 = fminf(rect1.y + rect1.h, rect2.y + rect2.h);

    Rect result = {
        .x = x1,
        .y = y1,
        .w = fmaxf(0.0f, x2 - x1),
        .h = fmaxf(0.0f, y2 - y1)
    };
    return result;
}

int rects_overlap(Rect rect1, Rect rect2)
{
    return rect1.x + rect1.w >= rect2.x
        && rect2.x + rect2.w >= rect1.x
        && rect2.y + rect2.h >= rect1.y
        && rect1.y + rect1.h >= rect2.y;
}

float line_length(Line line)
{
    float dx = line.p1.x - line.p2.x;
    float dy = line.p1.y - line.p2.y;
    return sqrtf(dx * dx + dy * dy);
}

void rect_object_impact(Rect object, Rect obstacle, int *sides)
{
    assert(sides);

    Rect int_area = rects_overlap_area(object, obstacle);

    if (int_area.w * int_area.h > 0.0f) {
        for (int side = 0; side < RECT_SIDE_N; ++side) {
            Line object_side = rect_side(object, side);
            Line int_side = rect_side(int_area, side);

            if (line_length(int_side) > 10.0f) {
                sides[side] = sides[side] ||
                    (fabs(object_side.p1.x - object_side.p2.x) < 1e-6
                     && fabs(object_side.p1.x - int_side.p1.x) < 1e-6
                     && fabs(object_side.p1.x - int_side.p2.x) < 1e-6)
                    || (fabs(object_side.p1.y - object_side.p2.y) < 1e-6
                        && fabs(object_side.p1.y - int_side.p1.y) < 1e-6
                        && fabs(object_side.p1.y - int_side.p2.y) < 1e-6);
            }
        }
    }
}

Line rect_side(Rect rect, Rect_side side)
{
    const float x1 = rect.x;
    const float y1 = rect.y;
    const float x2 = rect.x + rect.w;
    const float y2 = rect.y + rect.h;

    Line result;

    switch (side) {
    case RECT_SIDE_LEFT:
        result.p1.x = x1;
        result.p1.y = y1;
        result.p2.x = x1;
        result.p2.y = y2;
        break;
    case RECT_SIDE_RIGHT:
        result.p1.x = x2;
        result.p1.y = y1;
        result.p2.x = x2;
        result.p2.y = y2;
        break;
    case RECT_SIDE_TOP:
        result.p1.x = x1;
        result.p1.y = y1;
        result.p2.x = x2;
        result.p2.y = y1;
        break;
    case RECT_SIDE_BOTTOM:
        result.p1.x = x1;
        result.p1.y = y2;
        result.p2.x = x2;
        result.p2.y = y2;
        break;
    default: {}
    }

    return result;
}

Rect rect_from_point(Point p, float w, float h)
{
    Rect result = {
        .x = p.x,
        .y = p.y,
        .w = w,
        .h = h
    };

    return result;
}

int rect_contains_point(Rect rect, Point p)
{
    return rect.x <= p.x && p.x <= rect.x + rect.w
        && rect.y <= p.y && p.y <= rect.y + rect.h;
}

SDL_Rect rect_for_sdl(Rect rect)
{
    const SDL_Rect result = {
        .x = (int) roundf(rect.x),
        .y = (int) roundf(rect.y),
        .w = (int) roundf(rect.w),
        .h = (int) roundf(rect.h)
    };

    return result;
}
