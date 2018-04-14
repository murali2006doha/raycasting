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
			ray.intersection.obj_space = pt;
      ray.intersection.t_value = t_value;
      return true;
    }
    
  }
	
	return false;
}

bool UnitCompound::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	Point3D center(0, 0, 0.5);
	double t_value;

	Ray3D rayModel;
	rayModel.origin = worldToModel * ray.origin;
	rayModel.dir = worldToModel * ray.dir;

	Vector3D oc = rayModel.origin - center;

	//quadratic formula coeffecients
	double ac = rayModel.dir.dot(rayModel.dir);
	double bc = oc.dot(rayModel.dir);
	double cc = oc.dot(oc) - 1;
	double dc = bc * bc - ac * cc;
	bool ret = false;

  if(dc >= 0) {
    if(dc == 0) {
      t_value = - bc / ac;
    } else {
      double r1 = - (bc / ac) + (sqrt(dc) / ac);
      double r2 = - (bc / ac) - (sqrt(dc) / ac);
    
      t_value = fmin(r1, r2);
    }

    if( t_value > 0 && (ray.intersection.none || t_value < ray.intersection.t_value )) {

      Point3D pt = rayModel.origin + t_value * rayModel.dir;
      Vector3D norm = pt - center;
      norm.normalize();

      ray.intersection.point = modelToWorld * pt;
      ray.intersection.normal = worldToModel.transpose() * norm;
      ray.intersection.normal.normalize();
      ray.intersection.none = false;
      ray.intersection.t_value = t_value;
			ret = true;
    }
    
  }

	rayModel.origin = worldToModel * ray.origin;
	rayModel.dir = worldToModel * ray.dir;

	center = Point3D(0, 0, 0);
	Vector3D normal = Vector3D(0, 0, 1);


	//Check the for inter section with bottom 
	normal = Vector3D(0, 0, -1);
	if (rayModel.dir.dot(normal) != 0.0){
		double t2 = ((Point3D(0, 0, -0.5) - rayModel.origin).dot(normal)) / rayModel.dir.dot(normal);
		Point3D inter2 =  rayModel.origin + t2 * rayModel.dir;
		if (pow(inter2[0], 2) + pow(inter2[1], 2) < 1 && t2 > 0){
			if (ray.intersection.none || t2 < ray.intersection.t_value ){
				ray.intersection.point = modelToWorld * inter2;
				ray.intersection.normal = modelToWorld * normal;
				ray.intersection.none = false;
				ray.intersection.t_value = t2;
				ret = true;
			}
		}
	}

	//Check for the intersection with the rest of the cylinder
	Vector3D va = Vector3D(0, 0, 1);
	Point3D pa = Point3D(0, 0, -0.5);
	Vector3D dp = rayModel.origin - pa;

	//Getting coeffients for quadrtaic formula
	Vector3D a = rayModel.dir - rayModel.dir.dot(va) * va;
	Vector3D c = dp - dp.dot(va)* va;
	double A = pow(a.length(), 2);
	double B = 2 * (a.dot(c));
	double C = pow(c.length(), 2) - 1;

	double d = pow(B, 2) - 4 * A * C;
	if (d > 0){
		
		double t3 = (-B + sqrt(d))/(2 * A);
		double t4 = (-B - sqrt(d))/(2 * A);
		double t = fmin(t3, t4);
		if ( t > 0){
			
			Point3D inter3 = rayModel.origin + t * rayModel.dir;

			if ((ray.intersection.none || t < ray.intersection.t_value) && inter3[2] < 0.5 && inter3[2] > -0.5){
				
				ray.intersection.point = modelToWorld * inter3;
				ray.intersection.normal = modelToWorld * (inter3 - Point3D(0, 0, inter3[2]));
				ray.intersection.normal.normalize();
				ray.intersection.none = false;
				ray.intersection.t_value = t;
				ret = true;
			}
		}
	}


	return ret; 
}


bool UnitCylinder::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

	bool ret = false;
	Ray3D rayModel; 
	rayModel.origin = worldToModel * ray.origin;
	rayModel.dir = worldToModel * ray.dir;

	Point3D center(0, 0, 0);
	Vector3D normal = Vector3D(0, 0, 1);

	//Check the for inter section with top 
	normal = Vector3D(0, 0, 1);
	if (rayModel.dir.dot(normal) != 0.0){
		double t2 = ((Point3D(0, 0, 0.5) - rayModel.origin).dot(normal)) / rayModel.dir.dot(normal);
		Point3D inter2 =  rayModel.origin + t2 * rayModel.dir;
		if (pow(inter2[0], 2) + pow(inter2[1], 2) < 1 && t2 > 0){
			if (ray.intersection.none || t2 < ray.intersection.t_value ){
				ray.intersection.point = modelToWorld * inter2;
				ray.intersection.normal = modelToWorld * normal;
				ray.intersection.none = false;
				ray.intersection.t_value = t2;
				ret = true;
			}
		}
	}


	//Check the for inter section with bottom 
	normal = Vector3D(0, 0, -1);
	if (rayModel.dir.dot(normal) != 0.0){
		double t2 = ((Point3D(0, 0, -0.5) - rayModel.origin).dot(normal)) / rayModel.dir.dot(normal);
		Point3D inter2 =  rayModel.origin + t2 * rayModel.dir;
		if (pow(inter2[0], 2) + pow(inter2[1], 2) < 1 && t2 > 0){
			if (ray.intersection.none || t2 < ray.intersection.t_value ){
				ray.intersection.point = modelToWorld * inter2;
				ray.intersection.normal = modelToWorld * normal;
				ray.intersection.none = false;
				ray.intersection.t_value = t2;
				ret = true;
			}
		}
	}

	//Check for the intersection with the rest of the cylinder
	Vector3D va = Vector3D(0, 0, 1);
	Point3D pa = Point3D(0, 0, -0.5);
	Vector3D dp = rayModel.origin - pa;

	//Getting coeffients for quadrtaic formula
	Vector3D a = rayModel.dir - rayModel.dir.dot(va) * va;
	Vector3D c = dp - dp.dot(va)* va;
	double A = pow(a.length(), 2);
	double B = 2 * (a.dot(c));
	double C = pow(c.length(), 2) - 1;

	double d = pow(B, 2) - 4 * A * C;
	if (d > 0){
		
		double t3 = (-B + sqrt(d))/(2 * A);
		double t4 = (-B - sqrt(d))/(2 * A);
		double t = fmin(t3, t4);
		if ( t > 0){
			
			Point3D inter3 = rayModel.origin + t * rayModel.dir;

			if ((ray.intersection.none || t < ray.intersection.t_value) && inter3[2] < 0.5 && inter3[2] > -0.5){
				
				ray.intersection.point = modelToWorld * inter3;
				ray.intersection.normal = modelToWorld * (inter3 - Point3D(0, 0, inter3[2]));
				ray.intersection.normal.normalize();
				ray.intersection.none = false;
				ray.intersection.t_value = t;
				ret = true;
			}
		}
	}


	return ret; 
}

