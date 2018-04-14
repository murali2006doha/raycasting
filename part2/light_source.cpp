/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"

void PointLight::shade( Ray3D& ray ) {
	Material *mat = ray.intersection.mat;

	  Vector3D N = ray.intersection.normal;
	  Vector3D L = _pos - ray.intersection.point;
	  Vector3D V = -ray.dir;
	  Vector3D R = 2.* (L.dot(N) * N) - L;
	  N.normalize();
	  L.normalize();
	  V.normalize();
	  R.normalize();
	  
    Colour diffuseMat = mat->diffuse;

    if(ray.intersection.mat->isTexture) {
        
        Colour textColor = ray.intersection.mat->texture.getColour(ray.intersection.obj_space);
        
        diffuseMat = diffuseMat * textColor;
    }


    Colour ambient = (mat->ambient * _col_ambient);
    Colour diffuse = (fmax(0, N.dot(L)) * diffuseMat * _col_diffuse) ;
    Colour specular = (pow(fmax(0, V.dot(R)), mat->specular_exp) * mat->specular * _col_specular);

	  //Scene Signature
	  //ray.col= mat->diffuse;

	  //Diffuse
	  //ray.col = ambient + diffuse;

	  //Phong
	  ray.col = ambient + diffuse + specular;
	  

	  ray.col.clamp();
}

void PointLight::shadow( Ray3D& ray ) {
	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.

	Intersection inter = ray.intersection;
	Colour ambient = inter.mat->ambient * _col_ambient;

	ray.col = ambient;
	ray.col.clamp();
}

void SpotLight::shade( Ray3D& ray ) {
	Material *mat = ray.intersection.mat;

	  Vector3D N = ray.intersection.normal;
	  Vector3D L = _pos - ray.intersection.point;
	  Vector3D V = -ray.dir;
	  Vector3D R = 2.* (L.dot(N) * N) - L;
	  N.normalize();
	  L.normalize();
	  V.normalize();
	  R.normalize();
	  Colour ambient = (mat->ambient * _col_ambient);
	  Colour diffuse = (fmax(0, N.dot(L)) * mat->diffuse * _col_diffuse) ;
	  Colour specular = (pow(fmax(0, V.dot(R)), mat->specular_exp) * mat->specular * _col_specular);

	  //Scene Signature
	  //ray.col= mat->diffuse;

	  //Diffuse
	  //ray.col = ambient + diffuse;

	  //Phong
	  ray.col = ambient + diffuse + specular;
	  

	  ray.col.clamp();
}

void SpotLight::shadow(Ray3D & ray){
	Intersection inter = ray.intersection;
	Colour ambient = inter.mat->ambient * _col_ambient;

	ray.col = ambient;
	ray.col.clamp();
}

Point3D PointLight::get_random_point(){
	return _pos;
}

Point3D SpotLight::get_random_point(){
	float theta = 2 * M_PI * ((double) rand() / (RAND_MAX));
	Vector3D dir(cos(theta), sin(theta), 0);
	
	Point3D point = _pos + ((double) rand() / (RAND_MAX)) * dir;
	return point; 
}

