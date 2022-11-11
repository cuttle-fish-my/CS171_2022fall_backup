#ifndef BEZIER_H_
#define BEZIER_H_

#include "defines.h"
#include <object.h>

#include <vector>

class BezierCurve {
public:
    std::vector<vec3> control_points_;

    explicit BezierCurve(int m);

    explicit BezierCurve(std::vector<vec3> &control_points);

    Vertex evaluate(std::vector<vec3> &control_points, float t);

    Vertex evaluate(float t);

    Vertex evaluateBSpline(std::vector<vec3> &control_points, float t, size_t p);

    Vertex evaluateBSpline(float t, size_t p);

    std::vector<vec3> RecursiveAdaptive(vec3 p1, vec3 p2, vec3 p3, vec3 p4);

    std::vector<vec3> adaptive(std::vector<vec3> &control_points);

    Object generateObject(int I);

    static std::vector<float> generateKnot(size_t n, size_t p);

    Object generateBSplineObject(int I, size_t p);

    Object generateAdaptiveObject();

};


class BezierSurface {
public:
    std::vector<std::vector<vec3>> control_points_m_;
    std::vector<std::vector<vec3>> control_points_n_;

    BezierSurface(int m, int n);

    explicit BezierSurface(std::vector<std::vector<vec3>> &control_points);

    void setControlPoint(int i, int j, vec3 point);

    Vertex evaluate(std::vector<std::vector<vec3>> &control_points, float u, float v);

    Object generateObject(int I, int J);

    Vertex evaluateBSpline(std::vector<std::vector<vec3>> &control_points, float u, float v, size_t p);

    std::vector<std::vector<float>> adaptive(std::vector<std::vector<vec3>> &control_points, int I, int J);

    Object generateAdaptiveObject(int I, int J);

    static size_t count(BezierCurve curve, const std::vector<vec3>& points, float ul, float ur);

    Object generateBSplineObject(int I, int J, size_t p);

};

std::vector<BezierSurface> read(const std::string &path);

#endif