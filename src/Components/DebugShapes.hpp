#pragma once
#include <raylib.h>
#include <iostream>
#include <memory>

// Basic componentes
namespace rbvs
{
    /**
     * @brief Represents a 3D line with a starting position, ending position, and color.
     */
    struct Line
    {
        Vector3 start_pos; // Start position of the line.
        Vector3 end_pos;   // End position of the line.
        void draw(Color color);
    };

    /**
     * @brief Represents a 3D segement with a starting position, ending position, and color.
     */
    struct Segment
    {
        Vector3 start_pos; // Start position of the line.
        Vector3 end_pos;   // End position of the line.
        float thickness;   // Scale of the segement (thickness parameter)

        void draw(Color color);
    };

    /**
     * @brief Represents a 2D disc drawn in 3D space
     */
    struct Disc
    {
        // Vector3 center; // Start position of the line.
        // Vector3 axis;   // End position of the line.
        float radius;   // Radius of the ring
        // Color color = GREEN; // Color of the disc.

        void draw(Vector3 Poisition, Quaternion orientation, Color color);
    };

    /**
     * @brief Represents a 2D disc drawn in 3D space (with more features)
     */
    struct RingSection
    {
        //Vector3 center;      // Start position of the line.
        //Vector3 axis;        // End position of the line.
        float inner_radius;  // Radius of the ring
        float outer_radius;  // Radius of the ring
        float angle_f;       // Final angle
        float angle_o;       // Initial angle
        //Color color = GREEN; // Color of the disc.

        void draw(Vector3 Poisition, Quaternion orientation, Color color);

        // void draw()
        // {
        //     du::draw_ring_section(center, axis, outer_radius, inner_radius, angle_f, angle_o, color);
        // }
    };

    /**
     * @brief Represents a 3D sphere with a position, radius and color.
     */
    struct Sphere
    {
        float radius;     // Color of the sphere.
        void draw(Color color);
    };

    /**
     * @brief Represents a 3D arrow with an origin, vector, radius, and color.
     */
    struct Arrow
    {
        Vector3 origin; // Origin of the arrow.
        Vector3 vector; // Direction and length of the arrow.
        float radius;   // Radius of the arrow.
        Color color;    // Color of the arrow.
    };

    // Axis aligned bounding box
    struct AABB: BoundingBox
    {
        void draw(Color color);
    };

    /**
     * @brief Represents a 3D text label with text, position, font size, color, and background options.
     */
    struct TextLabel
    {
        std::string text;              // Text content of the label.
        Vector3 position;              // Position of the label.
        float fontSize = 500.0;        // Font size of the label.
        Color color = WHITE;           // Color of the label.
        Font font = GetFontDefault();  // Font used for rendering.
        bool background;               // Flag indicating whether to display a background for the label.
        Color backgroundColor = BLACK; // Background color.
        bool enabled = true;           // Flag indicating whether the label is enabled.
    };
};