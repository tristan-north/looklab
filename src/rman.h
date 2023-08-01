#pragma once

#include <QMatrix4x4>

void startRender();
void rmanSetCamXform(const QMatrix4x4& xformMat);
void rmanSetColorParam(char* paramName, float Cx, float Cy, float Cz); 
void rmanSetFloatParam(char* paramName, float x); 
void rmanSetStringParam(char* paramName, char* stringValue); 
void rmanSetIntParam(char* paramName, bool boolValue); 

