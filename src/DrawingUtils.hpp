#include <raylib.h>
#include <raymath.h>

namespace du
{
    void draw_arrow(Vector3 start_position, Vector3 end_position, Color color, float radius);

    void draw_axes(Vector3 position, Quaternion orientation, float scale = 1.0);

    /**
     * @brief Draws a line segment between to points p
     * @param p_1 : First point
     * @param P_2 : Second point
     * @param color : Color of the segment
     * @param scale : Scale of the segment (This paramenter control the radius of the cylinder and the )
    */
    void draw_segment(Vector3 p_1, Vector3 p_2, Color color, float scale);

    void draw_disc_section(Vector3 position, Vector3 axis, float radius, Color color);

    /**
     * @brief Draws a 2D Ring section in 3D space
     * @param position : Center of the ring
     * @param axis : Axis to wich the ring faces
     * @param r_1 : Smaller radius of the ring
     * @param r_2 : Bigger radius of the ring
     * @param angle_f : Final angle from where the ring section is drawn
     * @param angle_o : Staring angle from where the ring section is draw
     * @param color : Color of the segment
    */
    void draw_ring_section(Vector3 position, Vector3 axis, float r_1 , float r_2, float angle_f = 2*PI, float angle_o = 0.0, Color color = GREEN);

    /**
     * @brief Gets the 4x4 transformation matrix given the position vector and the orientation quaternion.
     * @param v Position vector.
     * @param q Orientation quaternion.
    */
    Matrix get_transform(const Vector3 &v, const Quaternion &q);
}
