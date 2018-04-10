#ifndef ARUCO_JAVA_H
#define ARUCO_JAVA_H

#include <jni.h>

#include <opencv2/core.hpp>
#ifdef __cplusplus
extern "C" {

// This should take an OpenCV picture matrix
// and return position and orientation relative to the camera.
JNIEXPORT void JNICALL Java_net_immersive_aruco
        (JNIEnv *, jclass, CvMat *) ;

}

/*
needed shit
 cvAruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_250);
 cv2.aruco.detectMarkers
 cv2.aruco.drawDetectedMarkers
cv2.aruco.estimatePoseSingleMarkers
 cv2.aruco.drawAxis

*/

#endif

#endif