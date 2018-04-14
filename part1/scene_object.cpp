/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <stdio.h>
#include <iostream>
#include "scene_object.h"

bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0), 
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	  Ray3D rayModel;
	  rayModel.origin = worldToModel * ray.origin;
	  rayModel.dir = worldToModel * ray.dir;

	  float t_value = -(rayModel.origin[2] / rayModel.dir[2]);
	  if(t_value < 0)
	    return false;

	  Point3D pt = rayModel.origin + t_value * rayModel.dir;
	  double x = pt[0];
	  double y = pt[1];

	  if(x > 0.5 || x < -0.5 || y > 0.5 || y < -0.5){
	    return false;
	  } else {
	    if (ray.intersection.none || t_value < ray.intersection.t_value){
	      ray.intersection.point = modelToWorld * pt;
	      ray.intersection.normal = worldToModel.transpose() * Vector3D(0, 0, 1);
	      ray.intersection.normal.normalize();
	      ray.intersection.none = false;
	      ray.intersection.t_value = t_value;
	      return true;
	    }
	  }

	return false;
}

bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSphere, which is centred 
	// on the origin.  
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	Point3D center(0, 0, 0);
	double t_value;

	Ray3D rayModel;
	rayModel.origin = worldToModel * ray.origin;
	rayModel.dir = worldToModel * ray.dir;

	Vector3D oc = rayModel.origin - center;

	//quadratic formula coeffecients
	double a = rayModel.dir.dot(rayModel.dir);
	double b = oc.dot(rayModel.dir);
	double c = oc.dot(oc) - 1;
	double d = b * b - a * c;
	//double b = 2 * (rayModel.dir.dot(oc));
	//double d = b * b - 4 * a * c;

  if(d >= 0) {
    if(d == 0) {
      t_value = - b / a;
    } else {
      double r1 = - (b / a) + (sqrt(d) / a);
      double r2 = - (b / a) - (sqrt(d) / a);
      t_value = fmin(r1, r2);

      if(t_value < 0) {
        t_value = fmax(r1, r2);
      }
    }

    if(t_value < 0) 
      return false;

    if(ray.intersection.none || t_value < ray.intersection.t_value) {

      Point3D pt = rayModel.origin + t_value * rayModel.dir;
      Vector3D norm = pt - center;
      norm.normalize();

      ray.intersection.point = modelToWorld * pt;
      ray.intersection.normal = worldToModel.transpose() * norm;
      ray.intersection.normal.normalize();
      ray.intersection.none = false;
      ray.intersection.t_value = t_value;
      return true;
    }
    
  }
	
	return false;
}




