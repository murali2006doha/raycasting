/***********************************************************
Starter code for Assignment 3

This code was originally written by Jack Wang for
CSC418, SPRING 2005

***********************************************************/


#include "raytracer.h"
#include <cstdlib>
#include <unistd.h>    /* for getopt */
#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */

int main(int argc, char* argv[])
{
    // Build your scene and setup your camera here, by calling 
    // functions from Raytracer.  The code here sets up an example
    // scene and renders it from two different view points, DO NOT
    // change this if you're just implementing part one of the 
    // assignment.  
    int c;
    bool shadows = false; 
    bool reflections = false; 
    bool antiAls = false; 
     while ( (c = getopt(argc, argv, "sra")) != -1) {
        int this_option_optind = optind ? optind : 1;
        switch (c) {
        case 'a':
            printf ("Anti Alasing Enabled\n");
            antiAls = true;
            break;
        case 's':
            printf ("Shadows Enabled\n");
            shadows = true;
            break;
        case 'r':
            printf ("Reflection enabled\n");
            reflections = true;
            break;
        default:
            printf ("?? getopt returned character code 0%o ??\n", c);
        }
    }


    Raytracer raytracer;
    //Set up render parameters 
    raytracer.renderAnti = antiAls; 
    raytracer.renderReflections = reflections; 
    raytracer.renderShadows = shadows; 

    int width = 320;
    int height = 240;

    if (argc == 3) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }

    // Camera parameters.
    Point3D eye(0, 0, 1);
    Vector3D view(0, 0, -1);
    Vector3D up(0, 1, 0);
    double fov = 60;

    // Defines a material for shading.
    Material gold(Colour(0.3, 0.3, 0.3), Colour(0.75164, 0.60648, 0.22648),
        Colour(0.628281, 0.555802, 0.366065),
        51.2);
    Material jade(Colour(0, 0, 0), Colour(0.54, 0.89, 0.63),
        Colour(0.316228, 0.316228, 0.316228),
        12.8, 1.5);

    Material moon( Colour(0.19225, 0.19225, 0.19225), Colour(0.50754, 0.50754, 0.50754),
        Colour(0.508273, 0.508273, 0.508273),
        51.20);

    Material metal( Colour(0.1, 0.1, 0.1), Colour(0.6, 0.6, 0.6), 
        Colour(0.7, 0.7, 0.3), 
        51.2 , 0.5);

    Texture texture("moon.bmp");
    moon.texture = texture;
    moon.isTexture = true;

    // Defines a point light source.
    raytracer.addLightSource(new PointLight(Point3D(0, 5, 5),
        Colour(0.9, 0.9, 0.9)));
       

    // Add a unit square into the scene with material mat.
    SceneDagNode* sphere = raytracer.addObject(new UnitSphere(), &moon);
    SceneDagNode* plane = raytracer.addObject(new UnitSquare(), &jade);
    //SceneDagNode* cylinder = raytracer.addObject(new UnitCylinder(), &gold);

    // Apply some transformations to the unit square.
    double factor1[3] = { 1.0, 1.0, 1.0 };
    double factor2[3] = { 8.0, 8.0, 8.0 };
    double factor3[3] = { 1.0, 1.0, 2.0 };

    raytracer.translate(sphere, Vector3D(2, 0, -5));
    raytracer.rotate(sphere, 'x', -45);
    raytracer.rotate(sphere, 'z', 45);
    raytracer.scale(sphere, Point3D(0, 0, 0), factor1);

    raytracer.translate(plane, Vector3D(0, 0, -7));
    raytracer.rotate(plane, 'z', 45);
    raytracer.scale(plane, Point3D(0, 0, 0), factor2);

    // raytracer.translate(cylinder, Vector3D(-2, 0, -5));
    // raytracer.scale(cylinder, Point3D(0, 0, 0), factor3);

    // Render the scene, feel free to make the image smaller for
    // testing purposes.    
    raytracer.render(width, height, eye, view, up, fov, "view1.bmp");

    // Render it from a different point of view.
    Point3D eye2(4, 2, 1);
    Vector3D view2(-4, -2, -6);
    raytracer.render(width, height, eye2, view2, up, fov, "view2.bmp");

    return 0;
}
