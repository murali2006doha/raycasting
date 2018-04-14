/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		Implementations of functions in raytracer.h, 
		and the main function which specifies the 
		scene to be rendered.	

***********************************************************/


#include "raytracer.h"
#include "bmp_io.h"
#include <cmath>
#include <iostream>
#include <cstdlib>

bool ANTIALIASING = true;

Raytracer::Raytracer() : _lightSource(NULL) {
	_root = new SceneDagNode();
}

Raytracer::~Raytracer() {
	delete _root;
}

SceneDagNode* Raytracer::addObject( SceneDagNode* parent, 
		SceneObject* obj, Material* mat ) {
	SceneDagNode* node = new SceneDagNode( obj, mat );
	node->parent = parent;
	node->next = NULL;
	node->child = NULL;
	
	// Add the object to the parent's child list, this means
	// whatever transformation applied to the parent will also
	// be applied to the child.
	if (parent->child == NULL) {
		parent->child = node;
	}
	else {
		parent = parent->child;
		while (parent->next != NULL) {
			parent = parent->next;
		}
		parent->next = node;
	}
	
	return node;;
}

LightListNode* Raytracer::addLightSource( LightSource* light ) {
	LightListNode* tmp = _lightSource;
	_lightSource = new LightListNode( light, tmp );
	return _lightSource;
}

void Raytracer::rotate( SceneDagNode* node, char axis, double angle ) {
	Matrix4x4 rotation;
	double toRadian = 2*M_PI/360.0;
	int i;
	
	for (i = 0; i < 2; i++) {
		switch(axis) {
			case 'x':
				rotation[0][0] = 1;
				rotation[1][1] = cos(angle*toRadian);
				rotation[1][2] = -sin(angle*toRadian);
				rotation[2][1] = sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'y':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][2] = sin(angle*toRadian);
				rotation[1][1] = 1;
				rotation[2][0] = -sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'z':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][1] = -sin(angle*toRadian);
				rotation[1][0] = sin(angle*toRadian);
				rotation[1][1] = cos(angle*toRadian);
				rotation[2][2] = 1;
				rotation[3][3] = 1;
			break;
		}
		if (i == 0) {
		    node->trans = node->trans*rotation; 	
			angle = -angle;
		} 
		else {
			node->invtrans = rotation*node->invtrans; 
		}	
	}
}

void Raytracer::translate( SceneDagNode* node, Vector3D trans ) {
	Matrix4x4 translation;
	
	translation[0][3] = trans[0];
	translation[1][3] = trans[1];
	translation[2][3] = trans[2];
	node->trans = node->trans*translation; 	
	translation[0][3] = -trans[0];
	translation[1][3] = -trans[1];
	translation[2][3] = -trans[2];
	node->invtrans = translation*node->invtrans; 
}

void Raytracer::scale( SceneDagNode* node, Point3D origin, double factor[3] ) {
	Matrix4x4 scale;
	
	scale[0][0] = factor[0];
	scale[0][3] = origin[0] - factor[0] * origin[0];
	scale[1][1] = factor[1];
	scale[1][3] = origin[1] - factor[1] * origin[1];
	scale[2][2] = factor[2];
	scale[2][3] = origin[2] - factor[2] * origin[2];
	node->trans = node->trans*scale; 	
	scale[0][0] = 1/factor[0];
	scale[0][3] = origin[0] - 1/factor[0] * origin[0];
	scale[1][1] = 1/factor[1];
	scale[1][3] = origin[1] - 1/factor[1] * origin[1];
	scale[2][2] = 1/factor[2];
	scale[2][3] = origin[2] - 1/factor[2] * origin[2];
	node->invtrans = scale*node->invtrans; 
}

Matrix4x4 Raytracer::initInvViewMatrix( Point3D eye, Vector3D view, 
		Vector3D up ) {
	Matrix4x4 mat; 
	Vector3D w;
	view.normalize();
	up = up - up.dot(view)*view;
	up.normalize();
	w = view.cross(up);

	mat[0][0] = w[0];
	mat[1][0] = w[1];
	mat[2][0] = w[2];
	mat[0][1] = up[0];
	mat[1][1] = up[1];
	mat[2][1] = up[2];
	mat[0][2] = -view[0];
	mat[1][2] = -view[1];
	mat[2][2] = -view[2];
	mat[0][3] = eye[0];
	mat[1][3] = eye[1];
	mat[2][3] = eye[2];

	return mat; 
}


void Raytracer::computeTransforms( SceneDagNode* node )
{
    SceneDagNode *childPtr;
    if (node->parent != NULL )
    {
        node->modelToWorld = node->parent->modelToWorld*node->trans;
        node->worldToModel = node->invtrans*node->parent->worldToModel; 
    }
    else
    {
        node->modelToWorld = node->trans;
        node->worldToModel = node->invtrans; 
    }
    // Traverse the children.
    childPtr = node->child;
    while (childPtr != NULL) {
        computeTransforms(childPtr);
        childPtr = childPtr->next;
    }



}

void Raytracer::traverseScene( SceneDagNode* node, Ray3D& ray ) {
    SceneDagNode *childPtr;

    // Applies transformation of the current node to the global
    // transformation matrices.
    if (node->obj) {
        // Perform intersection.
        if (node->obj->intersect(ray, node->worldToModel, node->modelToWorld)) {
            ray.intersection.mat = node->mat;
        }
    }
    // Traverse the children.
    childPtr = node->child;
    while (childPtr != NULL) {
        traverseScene(childPtr, ray);
        childPtr = childPtr->next;
    }

}

void Raytracer::computeShading( Ray3D& ray ) {
    
    LightListNode* curLight = _lightSource;
    for (;;) {
        if (curLight == NULL) break;
        // Each lightSource provides its own shading function.

        // Implement shadows here if needed.

		Vector3D dir = curLight->light->get_random_point() - ray.intersection.point;
		int light_t = dir.length();
		dir.normalize();
		Ray3D rayToLight = Ray3D(ray.intersection.point + 0.01 * dir, dir );
		traverseScene(_root, rayToLight);
		if (rayToLight.intersection.none == true || !renderShadows){
			curLight->light->shade(ray);
		}else{
			//Check if the intersection happens past the lightsource 
			if (rayToLight.intersection.t_value > light_t){
				curLight->light->shade(ray);
			}else{
				curLight->light->shadow(ray);
			}
			
		}
		
		curLight = curLight->next;
    }
}


void Raytracer::initPixelBuffer() {
    int numbytes = _scrWidth * _scrHeight * sizeof(unsigned char);
    _rbuffer = new unsigned char[numbytes];
    std::fill_n(_rbuffer, numbytes,0);
    _gbuffer = new unsigned char[numbytes];
    std::fill_n(_gbuffer, numbytes,0);
    _bbuffer = new unsigned char[numbytes];
    std::fill_n(_bbuffer, numbytes,0);
}

void Raytracer::flushPixelBuffer( std::string file_name ) {
    bmp_write( file_name.c_str(), _scrWidth, _scrHeight, _rbuffer, _gbuffer, _bbuffer );
    delete _rbuffer;
    delete _gbuffer;
    delete _bbuffer;
}

Colour Raytracer::shadeRay( Ray3D& ray, int depth ) {
    Colour col(0.0, 0.0, 0.0);
    Colour reflCol(0.0, 0.0, 0.0);
    traverseScene(_root, ray);

    // Don't bother shading if the ray didn't hit
    // anything.
    if (ray.intersection.none)
        return col;

    // TODO: You'll want to call shadeRay recursively (with a different ray,
    // of course) here to implement reflection/refraction effects.
    computeShading(ray);
    if (depth != 0) {

        Vector3D S = ray.intersection.point - ray.origin;
        Vector3D normal = ray.intersection.normal;
        S.normalize();

        // Reflection
		if (renderReflections){
			if (ray.intersection.mat->specular_exp > 0 && ray.intersection.mat->reflection_coeff > 0){
				Ray3D reflRay;
				reflRay.dir = S - 2 * ((S.dot(normal)) * normal);
				reflRay.dir.normalize();
				reflRay.origin = ray.intersection.point + 0.001 * reflRay.dir;
				reflCol = shadeRay(reflRay, depth - 1);
				ray.col = ray.col + ray.intersection.mat->reflection_coeff * ray.intersection.mat->specular * reflCol;
			}
		}
    }

    col = ray.col;
    col.clamp();
    return col;
}

void Raytracer::render( int width, int height, Point3D eye, Vector3D view, 
        Vector3D up, double fov, std::string fileName ) {
    computeTransforms(_root);
    Matrix4x4 viewToWorld;
    _scrWidth = width;
    _scrHeight = height;
    double factor = (double(height)/2)/tan(fov*M_PI/360.0);

    initPixelBuffer();
    viewToWorld = initInvViewMatrix(eye, view, up);

    // Construct a ray for each pixel.
    for (int i = 0; i < _scrHeight; i++) {
        for (int j = 0; j < _scrWidth; j++) {

            Colour col;

            //antialising
			if (renderAnti){
				for(float x = i; x < i + 1.0f; x+= 0.5f) {
					for(float y = j; y < j + 1.0f; y+= 0.5f) {

						Point3D origin(0, 0, 0);
						Point3D imagePlane;
						Ray3D ray;

						imagePlane[0] = (-double(width)/2 + 0.5 + y)/factor;
						imagePlane[1] = (-double(height)/2 + 0.5 + x)/factor;
						imagePlane[2] = -1;

						ray.origin = viewToWorld * origin;
						ray.dir = viewToWorld * (imagePlane - origin);
						ray.dir.normalize();

						col = shadeRay(ray, 99);

						_rbuffer[i*width+j] += int(col[0]*255 * 0.25f);
						_gbuffer[i*width+j] += int(col[1]*255 * 0.25f);
						_bbuffer[i*width+j] += int(col[2]*255 * 0.25f);
					}
				}
			}else {
				Point3D origin(0, 0, 0);
				Point3D imagePlane;
				Ray3D ray;

				// Sets up ray origin and direction in view space,
				// image plane is at z = -1.
				imagePlane[0] = (-double(width)/2 + 0.5 + j)/factor;
				imagePlane[1] = (-double(height)/2 + 0.5 + i)/factor;
				imagePlane[2] = -1;

				ray.origin = viewToWorld * origin;
				ray.dir = viewToWorld * (imagePlane - origin);
				ray.dir.normalize();

				col = shadeRay(ray, 99);

				_rbuffer[i*width+j] = int(col[0]*255);
				_gbuffer[i*width+j] = int(col[1]*255);
				_bbuffer[i*width+j] = int(col[2]*255);
			}
        }
    }

	flushPixelBuffer(fileName);
}

