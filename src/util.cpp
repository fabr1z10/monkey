#include <locale>
#include <codecvt>
#include "util.h"
#include <iostream>

std::u32string getString32(const std::string& str) {
    return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(str.c_str());
}

// returns the angle (in radians) between two vectors
float angle(glm::vec3 v1, glm::vec3 v2) {
    return std::acos(glm::dot(v1, v2));
}

float computeOverlap(glm::vec2& p1, glm::vec2& p2) {
    if (p2.x >= p1.x && p2.x <= p1.y) {
        if (p2.y > p1.y) {
            return -(p1.y - p2.x);
        }
    }
    else if (p1.x >= p2.x && p1.x <= p2.y) {
        if (p1.y > p2.y) {
            return (p2.y - p1.x);
        }
    }
    float a = p2.y - p1.x;
    float b = p1.y - p2.x;
    if (abs(a) < abs(b))
        return a;
    return -b;
}

float cross2d (glm::vec2 a, glm::vec2 b) {
    return a.x * b.y - a.y * b.x;
}

bool seg2segStrict(glm::vec2 A, glm::vec2 B , glm::vec2 C, glm::vec2 D) {
    float t{0.f};
    bool collide = seg2seg(A, B, C, D, t);
    return collide && t > 0.f && t < 1.f;

}

bool seg2seg(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D, float &t) {
    // eq for segment 1 is
    // Ax + t(Bx - Ax) = Cx + u(Dx - Cx)
    // Ay + t(By - Ay) = Cy + u(Dy - Cy)
    // which is
    // t(Bx - Ax) + u(Cx - Dx) = (Cx - Ax)
    // t(By - Ay) + u(Cy - Dy) = (Cy - Ay)
    // solution is
    //      | Cx - Ax    Dx - Cx |
    //      | Cy - Ay    Dy - Cy |
    // t = ------------------------
    //      | Bx - Ax    Dx - Cx |
    //      | By - Ay    Dy - Cy |
    glm::vec2 AB = B - A;
    glm::vec2 CD = C - D;
    float den = cross2d(AB, CD);
    if (isZero(den)) {
        // if den is zero -> AB || CD -> AB and CD are parallel or collineare.
        // IN both cases we treat this as no collision
        return false;
    }
    glm::vec2 AC = C - A;
    t = cross2d(AC, CD) / den;
    if (t >= 0.0f && t <= 1.0f) {
        float u = cross2d(AB, AC) / den;
        return (u >= 0.0f && u <= 1.0f);
    }
    return false;
}

// Solve using Cramer's rule
int solve2x2(float a11, float a12, float b1, float a21, float a22, float b2, float& x, float& y) {
    auto det = (a11 * a22 - a12 * a21);
    if (det == 0.f) {
        return 1;
    }
    auto dx = b1 * a22 - a12 * b2;
    auto dy = a11 * b2 - b1 * a21;
    auto idet = 1.0f / det;
    x = dx * idet;
    y = dy * idet;
    return 0;
}

float cross2D (glm::vec2 a, glm::vec2 b) {
    return a.x * b.y - a.y * b.x;
}

bool pnpoly(const std::vector<glm::vec2>& points, glm::vec2 p) {
    int i, j;
    bool c = false;
    int nvert = points.size();
    for (i = 0, j = nvert-1; i < nvert; j = i++) {
        if (points[i].y == p.y && points[j].y == p.y) {
            continue;
        }
        if ( ((points[i].y > p.y) != (points[j].y > p.y))) {
            float x =(points[j].x-points[i].x) * (p.y-points[i].y) / (points[j].y-points[i].y) + points[i].x;
            if (x == p.x) {
                return true;
            }
            if (p.x <= x) {
                c = !c;
            }
        }
    }
    return c;
}

glm::vec2 rot90(glm::vec2 vec, bool clockwise) {
    return (clockwise ? -1.f : 1.f) * glm::vec2(-vec.y, vec.x);
}

float shortestDistanceToSegment(glm::vec2 P0, glm::vec2 P1, glm::vec2 P2) {
    float den = (P2.x - P1.x) * (P2.x - P1.x) + (P2.y - P1.y) * (P2.y - P1.y);
    float num = (P2.x - P1.x) * (P1.y - P0.y) - (P1.x - P0.x) * (P2.y - P1.y);
    return (num * num) / den;
}


std::vector<glm::vec2> DouglasPeucker(std::vector<glm::vec2>& pointList, float epsilon, int i0, int i1) {
    // troviamo il punto con la massima distanza
    std::cout << " called DP with " << i0 << " --> " << i1 << "\n";
    float dmax = 0.f;
    int index = 0;
    auto end = i1 - i0 + 1;
    //se ha meno di 3 punti non vi è nulla da semplificare e torniamo l'array (di 1 o 2 punti)
    if (end < 3) {
        if (i0 == i1) {
            return {pointList[i0]};
        } else {
            return {pointList[i0], pointList[i1]};
        }
    }
    for (size_t i = i0 + 1; i < i1; i++) {
        float d2 = shortestDistanceToSegment(pointList[i], pointList[i0], pointList[i1]);
        if (d2 > dmax) {
            index = i;
            dmax = d2;
        }
    }
    // se la massima distanza è maggiore di epsilon, semplifichiamo ricorsivamente
    std::cout << " max sq dist is " << dmax << " at index " << index << ", " << pointList[index].x << ", " << pointList[index].y << "\n";
    std::vector<glm::vec2> resultList;
    if (dmax > epsilon*epsilon) {
        // Chiamate ricorsiva
        resultList = DouglasPeucker(pointList, epsilon, i0, index);
        auto recResults2 = DouglasPeucker(pointList, epsilon, index, i1);
        // Concateniamo le liste risultanti
        resultList.pop_back();
        resultList.insert( resultList.end(), recResults2.begin(), recResults2.end() );

    } else {
        //prendiamo i due punti estremi
        resultList = {pointList[i0], pointList[i1]};
    }
    // Ritorna il risultato
    return resultList;
}
