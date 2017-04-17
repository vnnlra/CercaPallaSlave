#ifndef TROVACERCHIO_H_INCLUDED
#define TROVACERCHIO_H_INCLUDED
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <cstdio>

using namespace cv;


/** Parametri per la trasformata di HOUGH
dp – Inverse ratio of the accumulator resolution to the image resolution.
    For example, if dp=1 , the accumulator has the same resolution as the
    input image. If dp=2 , the accumulator has half as big width and height.
minDist – Minimum distance between the centers of the detected circles. If the
    parameter is too small, multiple neighbor circles may be falsely detected
    in addition to a true one. If it is too large, some circles may be missed.
param1 – First method-specific parameter. In case of CV_HOUGH_GRADIENT ,
    it is the higher threshold of the two passed to the Canny() edge detector
    (the lower one is twice smaller).
param2 – Second method-specific parameter. In case of CV_HOUGH_GRADIENT , it is
    the accumulator threshold for the circle centers at the detection stage.
    The smaller it is, the more false circles may be detected. Circles,
    corresponding to the larger accumulator values, will be returned first.
minRadius – Minimum circle radius.
maxRadius – Maximum circle radius.
*/

/**
IL METODO CANNY
Canny
Step 1 – Calcolo Gradiente (Drog): magnitudo e direzione
Step 2 – Eliminazione dei punti il cui valore delgradiente non è superiore ai valori dei vicini
        (interpolazione lineare) nella direzione del gradiente
Step 3 – Sogliatura con Isteresi mediante 2 soglie T1 e T2
Step 4 – Edge linking per legare gli strong edge individuati con eventuali weak edge adiacenti
*/
/// Questi sono i parametri più importanti
extern int edgeThreshold; //param1
extern int accThreshold; //param2
/// Altri parametri trasformata
extern int accInvRatioResolution;
extern int minDistBetweenCenters;
///Altri parametri
extern int nBlur; /// dimensione del filtro di blur gaussiano
extern int minRaggio,maxRaggio; /// filtro sulle dimensioni del cerchio

extern char nomeFileConfHough[256]; ///nome del file comprensivo del path

int CercaUnCerchio(int camera, char* dest, bool ImmON);

#endif // TROVACERCHIO_H_INCLUDED
