#include <bezier.h>
#include <utils.h>
#include <vector>
#include <fstream>

BezierCurve::BezierCurve(int m) { control_points_.resize(m); }

BezierCurve::BezierCurve(std::vector<vec3> &control_points) {
    control_points_ = control_points;
}

Vertex BezierCurve::evaluate(std::vector<vec3> &control_points, float t) {
    control_points_ = control_points;
    std::vector<vec3> tmp = control_points;
    size_t n = control_points.size() - 1;
    Vertex res{};
    for (size_t k = 1; k <= n; ++k) {
        for (size_t i = 0; i <= n - k; i++) {
            if (i == n - k) {
                res.normal = tmp.at(i) - tmp.at(i + 1);
            }
            tmp.at(i) = (1 - t) * tmp.at(i) + t * tmp.at(i + 1);
        }
    }
    res.position = tmp.at(0);

    return res;
}

Vertex BezierCurve::evaluate(float t) {
    return evaluate(control_points_, t);
}

Object BezierCurve::generateObject(int I) {
    Object obj;
    for (int i = 0; i <= I; ++i) {
        float t = (float) i * 1 / (float) I;
        obj.vertices.push_back(evaluate(t));
    }
    obj.init();
    return obj;
}

Vertex BezierCurve::evaluateBSpline(std::vector<vec3> &control_points, float t, size_t p) {
    control_points_ = control_points;
    size_t s, k, h, n = control_points.size() - 1;

    if (t == 0.0f) {
        return Vertex{
                .position = control_points.at(0),
                .normal = control_points.at(0) - control_points.at(1)
        };
    }
    if (t == 1.0f) {
        return Vertex{
                .position = control_points.at(n),
                .normal = control_points.at(n - 1) - control_points.at(n)
        };
    }

    std::vector<float> knot = generateKnot(n, p);

    if (p == 1) {
        for (int i = 0; i < knot.size(); ++i) {
            if (t == knot.at(i)) {
                return Vertex{
                        .position = control_points.at(i - 1),
                        .normal = control_points.at(i - 1) - control_points.at(i)
                };
            }
        }
    }

    for (size_t i = 0; i < knot.size() - 1; ++i) {
        if (knot.at(i) <= t && knot.at(i + 1) > t) {
            s = knot.at(i) == t ? 1 : 0;
            k = i;
            h = p - s;
            break;
        }
    }

    std::vector<vec3> affected_points;
    for (size_t i = k - p; i <= k - s; ++i) {
        affected_points.push_back(control_points.at(i));
    }

    Vertex res{};

    for (size_t r = 1; r <= h; ++r) {
        for (size_t i = k - p + r; i <= k - s; ++i) {
            size_t j = i - (k - p + r);
            float a = (t - knot.at(i)) / (knot.at(i + p - r + 1) - knot.at(i));
            if (r == h) {
                res.normal = affected_points.at(j) - affected_points.at(j + 1);
            }
            affected_points.at(j) = (1 - a) * affected_points.at(j) + a * affected_points.at(j + 1);
        }
    }
    res.position = affected_points.at(0);
    return res;
}

Vertex BezierCurve::evaluateBSpline(float t, size_t p) {
    return evaluateBSpline(control_points_, t, p);
}

std::vector<float> BezierCurve::generateKnot(size_t n, size_t p) {
    std::vector<float> knot;
    size_t m = n + p + 1;
    knot.resize(m + 1);
    for (size_t i = 0; i <= p; ++i) {
        knot.at(i) = 0.0f;
        knot.at(m - i) = 1.0f;
    }
    for (size_t i = p + 1; i <= m - p - 1; ++i) {
        knot.at(i) = knot.at(i - 1) + 1 / (float) (m - 2 * p); // m - 2 * p = m + 1 - 2 * (p + 1) + 1
    }
    return knot;
}

Object BezierCurve::generateBSplineObject(int I, size_t p) {
    Object obj;
    for (int i = 0; i <= I; ++i) {
        float t = (float) i * 1 / (float) I;
        obj.vertices.push_back(evaluateBSpline(t, p));
    }
    obj.init();
    return obj;
}

std::vector<vec3> BezierCurve::RecursiveAdaptive(vec3 p1, vec3 p2, vec3 p3, vec3 p4) {
    double tolerance = 0.0025;
    vec3 p12 = (p1 + p2) / 2.0f;
    vec3 p23 = (p2 + p3) / 2.0f;
    vec3 p34 = (p3 + p4) / 2.0f;
    vec3 p123 = (p12 + p23) / 2.0f;
    vec3 p234 = (p23 + p34) / 2.0f;
    vec3 p1234 = (p123 + p234) / 2.0f;

    double d14 = glm::length(p4 - p1);

    double d2 = fabs(glm::length(glm::cross(p1 - p2, p4 - p1))) / d14;
    double d3 = fabs(glm::length(glm::cross(p3 - p4, p1 - p4))) / d14;

    if ((d2 + d3) * (d2 + d3) < tolerance * d14 * d14) {
        return std::vector<vec3>{p1234};
    }

    std::vector<vec3> left = RecursiveAdaptive(p1, p12, p123, p1234);
    std::vector<vec3> right = RecursiveAdaptive(p1234, p234, p34, p4);

    left.push_back(p1234);
    left.insert(left.end(), right.begin(), right.end());
    return left;
}

std::vector<vec3> BezierCurve::adaptive(std::vector<vec3> &control_points) {
    assert(control_points.size() == 4);
    std::vector<vec3> points = RecursiveAdaptive(
            control_points.at(0),
            control_points.at(1),
            control_points.at(2),
            control_points.at(3)
    );
    points.insert(points.begin(), control_points.at(0));
    points.push_back(control_points.at(3));
    return points;
}

Object BezierCurve::generateAdaptiveObject() {
    Object obj;
    assert(control_points_.size() == 4);
    std::vector<vec3> sample_points = adaptive(control_points_);
    for (auto point: sample_points) {
        obj.vertices.push_back(Vertex{point, vec3(0, 0, 0)});
    }
    obj.init();
    return obj;
}


BezierSurface::BezierSurface(int m, int n) {
    control_points_m_.resize(m);
    for (auto &sub_vec: control_points_m_) {
        sub_vec.resize(n);
    }
    control_points_n_.resize(n);
    for (auto &sub_vec: control_points_n_) {
        sub_vec.resize(m);
    }
}

BezierSurface::BezierSurface(std::vector<std::vector<vec3>> &control_points) {

    size_t m = control_points.size(), n = control_points[0].size();

    control_points_m_.resize(m);
    for (auto &sub_vec: control_points_m_) {
        sub_vec.resize(n);
    }

    control_points_n_.resize(n);
    for (auto &sub_vec: control_points_n_) {
        sub_vec.resize(m);
    }

    control_points_m_ = control_points;

    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            control_points_n_.at(j).at(i) = control_points_m_.at(i).at(j);
        }
    }

}

void BezierSurface::setControlPoint(int i, int j, vec3 point) {
    control_points_m_.at(i).at(j) = point;
    control_points_n_.at(j).at(i) = point;
}

Vertex BezierSurface::evaluate(std::vector<std::vector<vec3>> &control_points,
                               float u, float v) {
    std::vector<vec3> tmp;

    auto m = control_points.size();
    auto n = control_points.at(0).size();

    for (auto line: control_points_m_) {
        BezierCurve curve(line);
        tmp.push_back(curve.evaluate(line, v).position);
    }
    BezierCurve res_curve_m(tmp);
    Vertex res(res_curve_m.evaluate(tmp, u));

    tmp.clear();

    for (auto line: control_points_n_) {
        BezierCurve curve(line);
        tmp.push_back(curve.evaluate(line, u).position);
    }
    BezierCurve res_curve_n(tmp);
    vec3 normal = res_curve_n.evaluate(tmp, v).normal;
    res.normal = glm::normalize(glm::cross(res.normal, normal));

    return res;

}

Object BezierSurface::generateObject(int I, int J) {
    Object obj;
    for (int i = 0; i <= I; ++i) {
        float u = (float) i * 1 / (float) I;
        for (int j = 0; j <= J; ++j) {
            float v = (float) j * 1 / (float) J;
            obj.vertices.push_back(evaluate(control_points_m_, u, v));
        }
    }

    for (int i = 0; i < I; ++i) {
        for (int j = 0; j < J; ++j) {
            obj.indices.push_back(i * (J + 1) + j);
            obj.indices.push_back((i + 1) * (J + 1) + j);
            obj.indices.push_back((i + 1) * (J + 1) + (j + 1));

            obj.indices.push_back(i * (J + 1) + j);
            obj.indices.push_back(i * (J + 1) + (j + 1));
            obj.indices.push_back((i + 1) * (J + 1) + (j + 1));
        }
    }
    obj.init();
    return obj;
}

Vertex BezierSurface::evaluateBSpline(std::vector<std::vector<vec3>> &control_points, float u, float v, size_t p) {
    std::vector<vec3> tmp;

    auto m = control_points.size();
    auto n = control_points.at(0).size();

//    for (auto i = 0; i < m; ++i) {
//        for (auto j = 0; j < n; ++j) {
//            setControlPoint(i, j, control_points.at(i).at(j));
//        }
//    }

    for (auto line: control_points_m_) {
        BezierCurve curve(line);
        tmp.push_back(curve.evaluateBSpline(line, v, p).position);
    }
    BezierCurve res_curve_m(tmp);
    Vertex res(res_curve_m.evaluateBSpline(tmp, u, p));

    tmp.clear();

    for (auto line: control_points_n_) {
        BezierCurve curve(line);
        tmp.push_back(curve.evaluateBSpline(line, u, p).position);
    }
    BezierCurve res_curve_n(tmp);
    vec3 normal = res_curve_n.evaluateBSpline(tmp, v, p).normal;
    res.normal = glm::normalize(glm::cross(res.normal, normal));

    return res;
}

Object BezierSurface::generateBSplineObject(int I, int J, size_t p) {
    Object obj;
    for (int i = 0; i <= I; ++i) {
        float u = (float) i * 1 / (float) I;
        for (int j = 0; j <= J; ++j) {
            float v = (float) j * 1 / (float) J;
            obj.vertices.push_back(evaluateBSpline(control_points_m_, u, v, p));
        }
    }

    for (int i = 0; i < I; ++i) {
        for (int j = 0; j < J; ++j) {
            obj.indices.push_back(i * (J + 1) + j);
            obj.indices.push_back((i + 1) * (J + 1) + j);
            obj.indices.push_back((i + 1) * (J + 1) + (j + 1));

            obj.indices.push_back(i * (J + 1) + j);
            obj.indices.push_back(i * (J + 1) + (j + 1));
            obj.indices.push_back((i + 1) * (J + 1) + (j + 1));
        }
    }
    obj.init();
    return obj;
}

std::vector<std::vector<float>> BezierSurface::adaptive(std::vector<std::vector<vec3>> &control_points, int I, int J) {
    assert(control_points.size() == 4);
    assert(control_points.at(0).size() == 4);

    auto m = control_points.size();
    auto n = control_points.at(0).size();

    for (auto i = 0; i < m; ++i) {
        for (auto j = 0; j < n; ++j) {
            setControlPoint(i, j, control_points.at(i).at(j));
        }
    }

    std::vector<std::vector<vec3>> points;
    std::vector<BezierCurve> curves;

    std::vector<float> sample_points_m;
    std::vector<float> sample_points_n;
    std::vector<std::vector<float>> sample_points;

    for (auto line: control_points_m_) {
        BezierCurve curve(line);
        curves.push_back(curve);// each curve is n direction curve, not in m direction!!
        points.push_back(curve.adaptive(line)); // actually those are n-axis adaptive points
    }

//    count density for each u interval

    for (int i = 0; i < I; ++i) {
        float ul = (float) i * 1 / (float) I;
        float ur = (float) (i + 1) * 1 / (float) I;
        size_t max_density = 0;
        for (int j = 0; j < control_points_m_.size(); ++j) {
            size_t density = count(curves.at(j), points.at(j), ul, ur);
            max_density = std::max(max_density, density);
        }
        for (int j = 0; j < max_density; ++j) {
            sample_points_n.push_back((float) j * (ur - ul) / (float) max_density + ul);
        }
    }

    curves.clear();
    points.clear();
//    count density for each v interval

    for (auto line: control_points_n_) {
        BezierCurve curve(line);
        curves.push_back(curve);
        points.push_back(curve.adaptive(line));
    }

    for (int j = 0; j < J; ++j) {
        float vl = (float) j * 1 / (float) J;
        float vr = (float) (j + 1) * 1 / (float) J;
        size_t max_density = 0;
        for (int i = 0; i < control_points_n_.size(); ++i) {
            size_t density = count(curves.at(i), points.at(i), vl, vr);
            max_density = std::max(max_density, density);
        }
        for (int i = 0; i < max_density; ++i) {
            sample_points_m.push_back((float) i * (vr - vl) / (float) max_density + vl);
        }
    }

    sample_points.push_back(sample_points_m);
    sample_points.push_back(sample_points_n);

    return sample_points;
}

size_t BezierSurface::count(BezierCurve curve, const std::vector<vec3> &points, float ul, float ur) {
    vec3 p1 = curve.evaluate(ul).position;
    vec3 p2 = curve.evaluate(ur).position;
    vec3 v1 = p2 - p1;
    size_t num = 0;
    for (auto point: points) {
        vec3 v2 = point - p1;
        float ratio = glm::length(v2) / glm::length(v1);
        float angle = glm::dot(glm::normalize(v1), glm::normalize(v2));
        if (angle < 0) continue;
        if (ratio <= 1 && point != p2) num++;
        if (ratio <= 1 && point == points.back()) num++;
    }
    return num;
}

Object BezierSurface::generateAdaptiveObject(int I, int J) {
    Object obj;
    std::vector<std::vector<float>> sample_points = adaptive(control_points_m_, I, J);
    std::cout << "m direction sample points:\n";
    for (auto m: sample_points.at(0)) {
        std::cout << m << " ";
    }
    std::cout << std::endl;
    std::cout << "n direction sample points:\n";
    for (auto n: sample_points.at(1)) {
        std::cout << n << " ";
    }
    std::cout << std::endl;

    for (auto m: sample_points.at(0)) {
        for (auto n: sample_points.at(1)) {
            obj.vertices.push_back(evaluate(control_points_m_, m, n));
        }
    }

    auto len = sample_points.at(1).size();

    for (auto i = 0; i < sample_points.at(0).size() - 1; ++i) {
        for (auto j = 0; j < sample_points.at(1).size() - 1; ++j) {
            obj.indices.push_back(i * (len) + j);
            obj.indices.push_back((i + 1) * (len) + j);
            obj.indices.push_back((i + 1) * (len) + (j + 1));

            obj.indices.push_back(i * (len) + j);
            obj.indices.push_back(i * (len) + (j + 1));
            obj.indices.push_back((i + 1) * (len) + (j + 1));
        }
    }

    obj.init();
    return obj;
}


std::vector<BezierSurface> read(const std::string &path) {
    std::vector<BezierSurface> surfaces;
    std::ifstream infile(path);
    int b, p, m, n;
    infile >> b >> p >> m >> n;
    std::vector<std::vector<int>> indices;
    std::vector<vec3> points;
    for (int i = 0; i < b; ++i) {
        std::vector<int> line;
        for (int j = 0; j < m * n; ++j) {
            int idx;
            infile >> idx;
            line.push_back(idx);
        }
        indices.push_back(line);
    }

    for (int i = 0; i < p; ++i) {
        float x, y, z;
        infile >> x >> y >> z;
        points.emplace_back(x, y, z);
    }

    for (int i = 0; i < b; ++i) {
        std::vector<std::vector<vec3>> control_points;
        for (int r = 0; r < m; ++r) {
            std::vector<vec3> line;
            for (int c = 0; c < n; ++c) {
                line.push_back(points[indices.at(i).at(r * m + c)]);
            }
            control_points.push_back(line);
        }
        surfaces.emplace_back(control_points);
    }
    return surfaces;
}