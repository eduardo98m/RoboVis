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

    void draw_disc_section(Vector3 position, Vector3 axis, float radius, Color color)
    {
        int n_segments = 32;                  // Number of segments for the disc
        Vector3 points[4 * (n_segments + 1)]; // Adjusted size to include closing points
        float angle_step = 2.0f * PI / n_segments;

        // Normalize the axis to get the disc's orientation
        Vector3 z_axis = Vector3Normalize(axis);

        // Find two orthogonal vectors in the plane of the disc
        Vector3 x_axis;
        if (fabs(z_axis.x) < 0.001f && fabs(z_axis.z) < 0.001f)
        {
            x_axis = (Vector3){1, 0, 0}; // Handle the special case when z_axis is close to the y-axis
        }
        else
        {
            x_axis = Vector3Normalize(Vector3CrossProduct((Vector3){0, 1, 0}, z_axis)); // Perpendicular vector
        }
        Vector3 y_axis = Vector3CrossProduct(z_axis, x_axis); // Second perpendicular vector in the disc's plane

        // Generate points for the front and back faces, making a continuous strip
        for (int i = 0; i <= n_segments; i++)
        {
            float angle = i * angle_step;
            float x = cosf(angle) * radius;
            float y = sinf(angle) * radius;

            // Place the edge points in 3D space based on the disc orientation
            Vector3 edge_point = Vector3Add(
                Vector3Add(position, Vector3Scale(x_axis, x)), // Offset along x_axis
                Vector3Scale(y_axis, y));                      // Offset along y_axis

            // Front face (counter-clockwise winding)
            points[2 * i] = edge_point;   // Edge point for the front face
            points[2 * i + 1] = position; // Center point for the front face

            // Back face (clockwise winding to flip normal)
            points[2 * (n_segments + 1) + 2 * i] = position;       // Center point for the back face
            points[2 * (n_segments + 1) + 2 * i + 1] = edge_point; // Edge point for the back face
        }

        // Draw the full strip (both faces)
        DrawTriangleStrip3D(points, 4 * (n_segments + 1), color);
    }

    void draw_disc_section_2(Vector3 position, Vector3 axis, float radius, float angle_f, float angle_o, Color color)
    {
        int n_segments = 32;                  // Number of segments for the disc
        Vector3 points[4 * (n_segments + 1)]; // Adjusted size to include closing points
        float angle_step = (angle_f - angle_o) / n_segments;

        // Normalize the axis to get the disc's orientation
        Vector3 z_axis = Vector3Normalize(axis);

        // Find two orthogonal vectors in the plane of the disc
        Vector3 x_axis;
        if (fabs(z_axis.x) < 0.001f && fabs(z_axis.z) < 0.001f)
        {
            x_axis = (Vector3){1, 0, 0}; // Handle the special case when z_axis is close to the y-axis
        }
        else
        {
            x_axis = Vector3Normalize(Vector3CrossProduct((Vector3){0, 1, 0}, z_axis)); // Perpendicular vector
        }
        Vector3 y_axis = Vector3CrossProduct(z_axis, x_axis); // Second perpendicular vector in the disc's plane

        // Generate points for the front and back faces, making a continuous strip
        for (int i = 0; i <= n_segments; i++)
        {
            float angle = i * angle_step + angle_o;
            float x = cosf(angle) * radius;
            float y = sinf(angle) * radius;

            // Place the edge points in 3D space based on the disc orientation
            Vector3 edge_point = Vector3Add(
                Vector3Add(position, Vector3Scale(x_axis, x)), // Offset along x_axis
                Vector3Scale(y_axis, y));                      // Offset along y_axis

            // Front face (counter-clockwise winding)
            points[2 * i] = edge_point;   // Edge point for the front face
            points[2 * i + 1] = position; // Center point for the front face

            // Back face (clockwise winding to flip normal)            
            points[4 * (n_segments + 1) - 2 * (i + 1)] = position;     // Inner radius point for the back face (clockwise)
            points[4 * (n_segments + 1) - 2 * (i + 1) + 1] = edge_point; // Outer radius point for the back face (clockwis
        }

        // Draw the full strip (both faces)
        DrawTriangleStrip3D(points, 4 * (n_segments + 1), color);
    }

    void draw_ring_section(Vector3 position, Vector3 axis, float r_1, float r_2, float angle_f, float angle_o, Color color)
    {
        int n_segments = 32;                  // Number of segments for the disc
        Vector3 points[4 * (n_segments + 1)]; // Adjusted size to include closing points
        float angle_step = (angle_f - angle_o) / n_segments;

        // Normalize the axis to get the disc's orientation
        Vector3 z_axis = Vector3Normalize(axis);

        // Find two orthogonal vectors in the plane of the disc
        Vector3 x_axis;
        if (fabs(z_axis.x) < 0.001f && fabs(z_axis.z) < 0.001f)
        {
            x_axis = (Vector3){1, 0, 0}; // Handle the special case when z_axis is close to the y-axis
        }
        else
        {
            x_axis = Vector3Normalize(Vector3CrossProduct((Vector3){0, 1, 0}, z_axis)); // Perpendicular vector
        }
        Vector3 y_axis = Vector3CrossProduct(z_axis, x_axis); // Second perpendicular vector in the disc's plane

        // Generate points for the front and back faces, making a continuous strip
        for (int i = 0; i <= n_segments; i++)
        {
            float angle = i * angle_step + angle_o;

            float x_1 = cosf(angle) * r_1;
            float y_1 = sinf(angle) * r_1;

            float x_2 = cosf(angle) * r_2;
            float y_2 = sinf(angle) * r_2;

            // Place the edge points in 3D space based on the disc orientation
            Vector3 p_1 = Vector3Add(
                Vector3Add(position, Vector3Scale(x_axis, x_1)), // Offset along x_axis
                Vector3Scale(y_axis, y_1));                      // Offset along y_axis

            // Place the edge points in 3D space based on the disc orientation
            Vector3 p_2 = Vector3Add(
                Vector3Add(position, Vector3Scale(x_axis, x_2)), // Offset along x_axis
                Vector3Scale(y_axis, y_2));                      // Offset along y_axis

            // Front face (counter-clockwise winding)
            points[2 * i] = p_1;     // Edge point for the front face
            points[2 * i + 1] = p_2; // Center point for the front face

            // Back face (clockwise winding to flip normal)            
            points[4 * (n_segments + 1) - 2 * (i + 1)] = p_1;     // Inner radius point for the back face (clockwise)
            points[4 * (n_segments + 1) - 2 * (i + 1) + 1] = p_2; // Outer radius point for the back face (clockwise)
        }

        // Draw the full strip (both faces)
        DrawTriangleStrip3D(points, 4 * (n_segments + 1), color);
    }

    Matrix get_transform(const Vector3 &v, const Quaternion &q)
    {
        Vector3 axis;
        float angle;
        QuaternionToAxisAngle(q, &axis, &angle);
        Matrix rotationMatrix = MatrixRotate(axis, angle);
        Matrix translationMatrix = MatrixTranslate(v.x, v.y, v.z);
        return MatrixMultiply(rotationMatrix, translationMatrix);
    }

}
