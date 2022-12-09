#include "cloth.h"
#include "object.h"
#include "transform.h"

///////////////////
/// constructor ///
///////////////////

std::random_device randomDevice{};
std::mt19937 gen{randomDevice()};
std::uniform_real_distribution<float> ud(5.0f, 15.0f);
Vec3 SphereCenter(0, -1, 0);
float R = 0.52;

RectCloth::RectCloth(Float cloth_weight,
                     const UVec2 &mass_dim,
                     Float dx_local,
                     Float stiffness, Float damping_ratio, Camera *that_cam) :
        Mesh(std::vector<MeshVertex>(mass_dim.x * mass_dim.y),
             std::vector<UVec3>((mass_dim.y - 1) * (mass_dim.x - 1) * 2),
             GL_STREAM_DRAW, GL_STATIC_DRAW,
             true),
        // private
        mass_dim(mass_dim),
        mass_weight(cloth_weight / Float(mass_dim.x * mass_dim.y)),
        dx_local(dx_local),
        stiffness(stiffness),
        damping_ratio(damping_ratio),
        is_fixed_masses(mass_dim.x * mass_dim.y),
        local_or_world_positions(mass_dim.x * mass_dim.y),
        world_velocities(mass_dim.x * mass_dim.y),
        world_accelerations(mass_dim.x * mass_dim.y),
        cam(that_cam) {

    // initialize local positions
    const auto local_width = Float(mass_dim.x) * dx_local;
    const auto local_height = Float(mass_dim.y) * dx_local;

#pragma omp parallel for collapse(2)
    for (int ih = 0; ih < mass_dim.y; ++ih)
        for (int iw = 0; iw < mass_dim.x; ++iw)
            local_or_world_positions[Get1DIndex(iw, ih)] = Vec3(Float(iw) * dx_local - local_width * Float(0.5),
                                                                Float(ih) * dx_local - local_height * Float(0.5),
                                                                0);

    // initialize mesh vertices
    UpdateMeshVertices();

    // initialize mesh indices
#pragma omp parallel for collapse(2)
    for (int ih = 0; ih < mass_dim.y - 1; ++ih)
        for (int iw = 0; iw < mass_dim.x - 1; ++iw) {
            size_t i_indices = (size_t(ih) * size_t(mass_dim.x - 1) + size_t(iw)) << 1;

            auto i = Get1DIndex(iw, ih);
            auto r = Get1DIndex(iw + 1, ih);
            auto u = Get1DIndex(iw, ih + 1);
            auto ru = Get1DIndex(iw + 1, ih + 1);

            indices[i_indices] = UVec3(i, r, u);
            indices[i_indices + 1] = UVec3(r, ru, u);
        }
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) (sizeof(UVec3) * indices.size()), indices.data(),
                 GL_STATIC_DRAW);
    glBindVertexArray(0);
}



//////////////////
/// interfaces ///
//////////////////

bool RectCloth::SetMassFixedOrNot(int iw, int ih, bool fixed_or_not) {
    iw = iw < 0 ? int(mass_dim.x) + iw : iw;
    ih = ih < 0 ? int(mass_dim.y) + ih : ih;

    size_t idx;
    if (!Get1DIndex(iw, ih, idx))
        return false;

    is_fixed_masses[idx] = fixed_or_not;
    return true;
}

/*override*/ void RectCloth::FixedUpdate() {
    // simulate
    Simulate(simulation_steps_per_fixed_update_time);

    // update mesh vertices
    UpdateMeshVertices();
}



/////////////////////////
/// force computation ///
/////////////////////////

Vec3 RectCloth::ComputeHookeForce(int iw_this, int ih_this,
                                  int iw_that, int ih_that,
                                  Float dx_world) const {

    /*! TODO: implement this: compute the force according to Hooke's law
     *                        applied to mass(iw_this, ih_this)
     *                                by mass(iw_that, ih_that)
     *                        `dx_world` is "the zero-force distance" in world coordinate
     *
     *        note: for invalid `iw` or `ih`, you may simply return { 0, 0, 0 }
     */
    size_t this_idx, that_idx;
    if (!Get1DIndex(iw_this, ih_this, this_idx) || !Get1DIndex(iw_that, ih_that, that_idx)) {
        return {0, 0, 0};
    }
    Vec3 p = local_or_world_positions.at(this_idx);
    Vec3 q = local_or_world_positions.at(that_idx);
    Vec3 dis = p - q;

    return stiffness * (dx_world - glm::length(dis)) * glm::normalize(dis);
}

Vec3 RectCloth::ComputeSpringForce(int iw, int ih) const {

    const Vec3 scale = object->transform->scale;

    /*! TODO: implement this: compute the total spring force applied to mass(iw, ih)
     *                        by some other neighboring masses
     *
     *        note: you MUST consider structural, shear, and bending springs
     *              you MUST consider scaling of "the zero-force distance"
     *              you may find ComputeHookeForce() helpful
     *              for invalid `iw` or `ih`, you may simply return { 0, 0, 0 }
     *              for "fixed masses", you may also simply return { 0, 0, 0 }
     */

    Vec3 springForce(0);
    size_t idx;
    if (!Get1DIndex(iw, ih, idx) || is_fixed_masses.at(idx)) {
        return springForce;
    }

//    structural

    springForce += ComputeHookeForce(iw, ih, iw, ih - 1, dx_local);
    springForce += ComputeHookeForce(iw, ih, iw, ih + 1, dx_local);
    springForce += ComputeHookeForce(iw, ih, iw - 1, ih, dx_local);
    springForce += ComputeHookeForce(iw, ih, iw + 1, ih, dx_local);

//    shear

    springForce += ComputeHookeForce(iw, ih, iw - 1, ih - 1, (float) std::sqrt(2) * dx_local);
    springForce += ComputeHookeForce(iw, ih, iw - 1, ih + 1, (float) std::sqrt(2) * dx_local);
    springForce += ComputeHookeForce(iw, ih, iw + 1, ih - 1, (float) std::sqrt(2) * dx_local);
    springForce += ComputeHookeForce(iw, ih, iw + 1, ih + 1, (float) std::sqrt(2) * dx_local);

//    bending

    springForce += ComputeHookeForce(iw, ih, iw, ih - 2, 2 * dx_local);
    springForce += ComputeHookeForce(iw, ih, iw, ih + 2, 2 * dx_local);
    springForce += ComputeHookeForce(iw, ih, iw - 2, ih, 2 * dx_local);
    springForce += ComputeHookeForce(iw, ih, iw + 2, ih, 2 * dx_local);

    return springForce;
}



///////////////////////////
/// simulation pipeline ///
///////////////////////////

void RectCloth::LocalToWorldPositions() {

    const Mat4 model_matrix = object->transform->ModelMat();

    /*! TODO: implement this: transform mass positions from local coordinate to world coordinate
     *
     *        note: you may find `class Transform` in `transform.h` helpful
     */

    for (auto &pos: local_or_world_positions) {
        pos = Vec3(model_matrix * Vec4(pos, 1));
    }
}

void RectCloth::ComputeAccelerations() {

    /*! TODO: implement this: compute accelerations for each mass
     *
     *        note: you may find ComputeSpringForce() helpful
     *              you may store the results into `world_accelerations`
     */

    std::normal_distribution<float> nd(ud(gen), 2 * std::max(ud(gen), 5.0f));
    for (int w = 0; w < mass_dim.x; ++w) {
        for (int h = 0; h < mass_dim.y; ++h) {
            size_t idx;
            if (Get1DIndex(w, h, idx) && !is_fixed_masses.at(idx)) {
                world_accelerations.at(idx) =
                        (ComputeSpringForce(w, h) - damping_ratio * world_velocities.at(idx)) / mass_weight;
                if (!is_fixed_masses.at(idx)) {
                    world_accelerations.at(idx) += Vec3(0, -9.8, 0);
                    world_accelerations.at(idx) += Vec3(0, 0, nd(gen));
                }
            }
        }
    }
}

void RectCloth::ComputeVelocities() {

    /*! TODO: implement this: update velocities for each mass
     *
     *        note: you may store the results into `world_velocities`
     *              you may use `this->fixed_delta_time` instead of `Time::fixed_delta_time`, why?
     */
    size_t length = world_velocities.size();
    for (size_t i = 0; i < length; ++i) {
        world_velocities.at(i) += world_accelerations.at(i) * RectCloth::fixed_delta_time;
        if (glm::distance(local_or_world_positions.at(i), SphereCenter) < R) {
            Vec3 normal = glm::normalize(local_or_world_positions.at(i) - SphereCenter);
            local_or_world_positions.at(i) = SphereCenter + R * normal;
            Vec3 v_n_component = glm::dot(world_velocities.at(i), normal) * normal;
            world_velocities.at(i) -= v_n_component;
        }
    }

    if (drag_idx != -1 && Input::GetMouseButton(0)) {
        Vec3 p = local_or_world_positions.at(drag_idx);
        world_velocities.at(drag_idx) = Vec3(0);
        local_or_world_positions.at(drag_idx) = GetDragPos(p);
        return;
    }

    if (Input::GetMouseButton(0)) {
        Vec3 dir = glm::normalize(GetWorldPosFromCursor() - cam->transform.position);
        for (size_t i = 0; i < length; ++i) {
            if (Interact(cam->transform.position, dir, i)) {
                world_velocities.at(i) = Vec3(0);
                drag_idx = (int) i;
                break;
            }
        }
    } else {
        drag_idx = -1;
    }

}

void RectCloth::ComputePositions() {

    /*! TODO: implement this: update positions for each mass
     *
     *        note: you may store the results into `local_or_world_positions`
     *              you may use `this->fixed_delta_time` instead of `Time::fixed_delta_time`, why?
     */
    size_t length = local_or_world_positions.size();
    for (size_t i = 0; i < length; ++i) {
        local_or_world_positions.at(i) += world_velocities.at(i) * RectCloth::fixed_delta_time;
    }
}

void RectCloth::WorldToLocalPositions() {

    const Mat4 model_matrix = glm::inverse(object->transform->ModelMat());

    /*! TODO: implement this: transform mass positions from world coordinate to local coordinate
     *
     *        note: you may find `class Transform` in `transform.h` helpful
     */
    for (auto &pos: local_or_world_positions) {
        pos = Vec3(model_matrix * Vec4(pos, 1));
    }
}

void RectCloth::Simulate(unsigned num_steps) {
    for (unsigned i = 0; i < num_steps; ++i) {
        LocalToWorldPositions();
        ComputeAccelerations();
        ComputeVelocities();
        ComputePositions();
        WorldToLocalPositions();
    }
}



/////////////////
/// rendering ///
/////////////////

void RectCloth::UpdateMeshVertices() {

    // set vertex positions
    for (size_t i = 0; i < local_or_world_positions.size(); ++i)
        vertices[i].position = local_or_world_positions[i];

    // reset vertex normals
    auto compute_normal = [&](auto v1, auto v2, auto v3) {
        return glm::normalize(glm::cross(vertices[v2].position - vertices[v1].position,
                                         vertices[v3].position - vertices[v1].position));
    };

#pragma omp parallel for collapse(2)
    for (int ih = 0; ih < mass_dim.y; ++ih)
        for (int iw = 0; iw < mass_dim.x; ++iw) {
            constexpr auto w_small = Float(0.125);
            constexpr auto w_large = Float(0.25);

            auto i = Get1DIndex(iw, ih);
            auto l = Get1DIndex(iw - 1, ih);
            auto r = Get1DIndex(iw + 1, ih);
            auto u = Get1DIndex(iw, ih + 1);
            auto d = Get1DIndex(iw, ih - 1);
            auto lu = Get1DIndex(iw - 1, ih + 1);
            auto rd = Get1DIndex(iw + 1, ih - 1);
            auto &normal = vertices[i].normal;

            normal = {0, 0, 0};

            if (iw > 0 && ih < mass_dim.y - 1) {
                normal += compute_normal(l, i, lu) * w_small;
                normal += compute_normal(i, u, lu) * w_small;
            }
            if (iw < mass_dim.x - 1 && ih < mass_dim.y - 1) {
                normal += compute_normal(i, r, u) * w_large;
            }
            if (iw > 0 && ih > 0) {
                normal += compute_normal(l, d, i) * w_large;
            }
            if (iw < mass_dim.x - 1 && ih > 0) {
                normal += compute_normal(d, rd, i) * w_small;
                normal += compute_normal(rd, r, i) * w_small;
            }

            normal = glm::normalize(normal);
        }

    // vbo
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (sizeof(MeshVertex) * vertices.size()), vertices.data(),
                 buffer_data_usage_vbo);
    glBindVertexArray(0);
}



//////////////////////////
/// supporting methods ///
//////////////////////////

size_t RectCloth::Get1DIndex(int iw, int ih) const {
    return size_t(ih) * size_t(mass_dim.x) + size_t(iw);
}

bool RectCloth::Get1DIndex(int iw, int ih, size_t &idx) const {
    if (iw < 0 || ih < 0 || iw >= mass_dim.x || ih >= mass_dim.y)
        return false;
    idx = size_t(ih) * size_t(mass_dim.x) + size_t(iw);
    return true;
}


Vec3 RectCloth::GetWorldPosFromCursor() {
    Mat4 projection = glm::perspective(glm::radians(cam->fov_y), cam->aspect, cam->near, cam->far);
    Vec4 viewport{0, 0, 1920, 1080};
    return glm::unProject(Input::mouse_position, cam->LookAtMat(), projection, viewport);
}

bool RectCloth::Interact(Vec3 origin, Vec3 dir, size_t idx) {
    /*
     * dir must be normalized
     * */

    float radius = 0.05f;
    Vec3 center = local_or_world_positions.at(idx);
    float d_1 = glm::dot((center - origin), dir);
    float dis = std::sqrt(glm::distance(center, origin) * glm::distance(center, origin) - d_1 * d_1);
    return dis <= radius;
}

Vec3 RectCloth::GetDragPos(Vec3 p) {
    Vec3 normal = glm::normalize(cam->transform.Forward());
    Vec3 origin = cam->transform.position;
    Vec3 dir = glm::normalize(GetWorldPosFromCursor() - origin);
    float t = glm::dot(normal, (p - origin)) / glm::dot(normal, dir);
    return origin + t * dir;
}


