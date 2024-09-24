#include "DrawingUtils.hpp"

namespace du
{
    void draw_arrow(Vector3 start_position, Vector3 end_position, Color color, float radius)
    {

        Vector3 dir_vector = Vector3Subtract(end_position, start_position);
        Vector3 tip_start_pos = Vector3Add(start_position, Vector3Scale(dir_vector, 0.9));
        float distance = Vector3Distance(start_position, end_position);

        DrawCylinderEx(start_position, tip_start_pos, radius, radius, 10, color);
        DrawCylinderEx(tip_start_pos, end_position, radius * 2.0, 0.01 * radius, 20, color);
    }

    void draw_axes(Vector3 position, Quaternion orientation, float scale)
    {
        float lenght = 1.0 * (scale < 0.0 ? 0.0 : scale);
        float radius = lenght * 0.03;
        orientation = QuaternionNormalize(orientation);
        draw_arrow(position, Vector3Add(position, Vector3RotateByQuaternion({lenght, 0.0, 0.0}, orientation)), RED, radius);
        draw_arrow(position, Vector3Add(position, Vector3RotateByQuaternion({0.0, lenght, 0.0}, orientation)), GREEN, radius);
        draw_arrow(position, Vector3Add(position, Vector3RotateByQuaternion({0.0, 0.0, lenght}, orientation)), BLUE, radius);
    }
    
    void draw_segment(Vector3 p_1, Vector3 p_2, Color color, float scale)
    {
        float radius = scale * 0.03;

        DrawCylinderEx(p_1, p_2, radius, radius, 10, color);
        DrawSphere(p_1, radius * 1.5, color);
        DrawSphere(p_2, radius * 1.5, color);

    }

    Matrix get_transform(const Vector3 &v, const Quaternion &q){
        Vector3 axis;
        float angle;
        QuaternionToAxisAngle(q, &axis, &angle);
        Matrix rotationMatrix    = MatrixRotate(axis, angle);
        Matrix translationMatrix = MatrixTranslate(v.x, v.y, v.z);
        return MatrixMultiply(rotationMatrix, translationMatrix);

    }

}
