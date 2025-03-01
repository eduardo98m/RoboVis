/**
 * @brief 
 */
#pragma once
#include <raylib.h>
#include <iostream>
#include <memory>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

// Basic componentes
namespace rbvs
{
    struct VisualModel
    {
        Vector3 position;
        Quaternion orientation;
        Vector3 scale;
        Color color;
        std::unique_ptr<Model> model;
        bool visible = true;
    };


    struct DebugShape
    {
        Vector3 position;
        Quaternion orientation;
        Vector3 scale;
        Color color;
        float lifetime;
    };

    struct TextLabel
    {
        Vector3 position;
        Quaternion orientation;
        Vector3 scale;
        Color color;
        float lifetime;
    };

    struct PointCloud
    {
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud;
        Vector3 position;
        Quaternion orientation;
        Color color;
        bool visible = true;
        int ssboID = 0;

        //PointCloud() : cloud(new pcl::PointCloud<pcl::PointXYZ>) {}
    };
        
}
