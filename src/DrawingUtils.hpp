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

    Matrix get_transform(const Vector3 &v, const Quaternion &q);
}
