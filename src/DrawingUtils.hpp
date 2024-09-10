#include <raylib.h>
#include <raymath.h>

namespace du
{
    void draw_arrow(Vector3 start_position, Vector3 end_position, Color color, float radius);

    void draw_axes(Vector3 position, Quaternion orientation, float scale = 1.0);

    Matrix get_transform(const Vector3 &v, const Quaternion &q);
}
