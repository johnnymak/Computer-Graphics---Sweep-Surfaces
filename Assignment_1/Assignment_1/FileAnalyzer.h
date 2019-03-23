
/*****
*
*	Johnny Mak
*	Comp 371 - Computer Graphics
*	Assignment #1
*
*****/


#ifndef FILE_ANALYZER_H
#define FILE_ANALYZER_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>

#include "TranslationalObject.h"
#include "RotationalObject.h"

// GLEW 
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;


class FileAnalyzer {

	public:
		int profileCurveLength;
		int trajectoryCurveLength;
		vector<GLfloat> vertices;
		bool isTranslational;
		int rotationSpan;

		FileAnalyzer(string fileLocation) {

			string line;
			float x, y, z;
			int count;
			

			ifstream myfile(fileLocation);

			if (myfile.is_open()) {

				// Checks for the type of transformation
				getline(myfile, line);
				float type = atof(line.c_str());

				// Translational Object
				if (type == 0) {

					isTranslational = true;
					TranslationalObject obj;

					// Gets Number of Profile Curve Coordinates
					getline(myfile, line);
					count = atof(line.c_str());

					for (int i = 0; i < count; i++) {
						myfile >> x >> y >> z;
						obj.addProfileCurve(x, y, z);
						getline(myfile, line);
					}

					// Gets Number of Trajectory Curve Coordinates
					getline(myfile, line);
					count = atof(line.c_str());

					for (int i = 0; i < count; i++) {
						myfile >> x >> y >> z;
						obj.addTrajectoryCurve(x, y, z);
						getline(myfile, line);
					}

					glm::vec3 distance;
					profileCurveLength    = obj.getProfileCurveSize();
					trajectoryCurveLength = obj.getTrajectoryCurveSize();
					

					// Calculate the distance for each trajectory point, Translate, and Store 
					for (int i = 0; i < obj.getTrajectoryCurveSize(); i++) {
						distance = findDistance(obj.getLastProfileCurveCoordinates(), obj.getTrajectoryCurveCoordinates(i));


						for (int j = 0; j < obj.getProfileCurveSize(); j++) {

							glm::vec4 translatedProfileCurve;
							glm::mat4 trans;
							trans = glm::translate(trans, distance);

							translatedProfileCurve = trans * obj.getProfileCurveCoordinates(j);

							vertices.push_back(ceil(translatedProfileCurve.x * 100) / 100);
							vertices.push_back(ceil(translatedProfileCurve.y * 100) / 100);
							vertices.push_back(ceil(translatedProfileCurve.z * 100) / 100);
						}
					}

				}

				// Does Rotational Object
				else if (type == 1) {

					isTranslational = false;
					RotationalObject obj;

					// Get Number of Rotation Span
					getline(myfile, line);
					rotationSpan = atof(line.c_str());

					// Gets Number of Profile Curve Coordinates
					getline(myfile, line);
					count = atof(line.c_str());

					for (int i = 0; i < count; i++) {
						myfile >> x >> y >> z;
						obj.addProfileCurve(x, y, z);
						getline(myfile, line);
					}

					profileCurveLength = obj.getProfileCurveSize();


					// Rotates the vertices
					for (int i = 0 ; i <= rotationSpan; i++) {
					
						for (int j = 0; j < obj.getProfileCurveSize(); j++) {

							if (i == 0) {
								vertices.push_back(obj.getProfileCurveCoordinates(j).x);
								vertices.push_back(obj.getProfileCurveCoordinates(j).y);
								vertices.push_back(obj.getProfileCurveCoordinates(j).z);

								// cout << "IN HERE " << obj.getProfileCurveCoordinates(j).x << " " << obj.getProfileCurveCoordinates(j).y << " " << obj.getProfileCurveCoordinates(j).z << " "  << endl;
							}

							else {

								glm::vec4 rotatedvertex;
								glm::mat4 rotate;
								rotate = glm::rotate(rotate, glm::radians((float)(360 / rotationSpan)), glm::vec3(0.0f, 0.0f, 1.0f));

								rotatedvertex = rotate * obj.getProfileCurveCoordinates(j);

								vertices.push_back(ceil(rotatedvertex.x * 100) / 100);
								vertices.push_back(ceil(rotatedvertex.y * 100) / 100);
								vertices.push_back(ceil(rotatedvertex.z * 100) / 100);

								obj.setNewProfileCurveVertex(j, rotatedvertex);
							}
						}
					}	

				}

				else
					cout << "Invalid file configuration" << endl;
				
				myfile.close();
			}

			else 
				cout << "Unable to open file";

		}

		glm::vec3 findDistance(glm::vec4 pCurve, glm::vec3 tCurve) {
			glm::vec3 distance(tCurve.x - pCurve.x, tCurve.y - pCurve.y, tCurve.z - pCurve.z);

			return distance;
		}

		GLfloat getVertex(int index) {
			return vertices.at(index);
		}

		int getVertexSize() {
			return vertices.size();
		}

		int getProfileCurveLength() {
			return profileCurveLength;
		}

		int getTrajectoryCurveLength() {
			return trajectoryCurveLength;
		}

		int getRotationSpan() {
			return rotationSpan;
		}
		
		bool isObjectTranslational() {
			return isTranslational;
		}



		// TEST METHOD: Method used for testing purposes
		void printVertexArray() {

			cout << "Size of the vertex array: " << vertices.size() << endl;

			for (int i = 0; i < vertices.size(); i++) {
				
				if (i % 3 == 0)
					cout << endl;

				cout << vertices.at(i) << " ";
			}

			cout << endl;
		}

};

#endif