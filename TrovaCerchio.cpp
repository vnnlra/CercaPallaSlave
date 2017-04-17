#include <fstream>
#include "SerialeIO.h"
#include "TrovaCerchio.h"

using namespace std;

/**
    Parametro globale per il nome dei file di configurazione della ricerca

*/
char nomeFileConfHough[256];

/**
PARAMETRI GLOBALI DEL CERCHIO: INVIATI QUANDO RICHIESTI SULLA SERIALE
*/
int x=-1,y=-1,r=-1;
/// Questi sono i parametri più importanti: edgeThreshold e accThreshold
int edgeThreshold=80; //param1
/**
soglia sull'accumulatore nella trasformata di hough. Ogni punto nel piano di Hough rappresenta una retta,
l'accumulatore su quel punto coincide con il numero di punti nell'immagine che appartengono a quella retta
*/
int accThreshold=60;
/// Altri parametri traformata di
int accInvRatioResolution=1;
int minDistBetweenCenters=10;
///Altri parametri
int nBlur=3; /// dimensione del filtro di blur gaussiano
///Parametri dimensioni dei cerchi
int minRaggio=120, maxRaggio=140;

int leggeParametriCalibrazione(char *nomefile)
{
    string stringa;
    fstream fileConf(nomefile,ios::in);
    if(fileConf.fail())
        return -1;
    fileConf >> stringa >> edgeThreshold;
    fileConf >> stringa >> accThreshold;
    fileConf >> stringa >> accInvRatioResolution;
    fileConf >> stringa >> minDistBetweenCenters;
    fileConf >> stringa >> nBlur;
    fileConf >> stringa >> minRaggio;
    fileConf >> stringa >> maxRaggio;
    fileConf.close();
    return 0;
}

/**
    Cerca UN cerchio
    Esegue il loop di acquisizione dell'immagine e cerca un cerchio le cui dimensioni sono
    comprese tra minRaggio e maxRaggio
    Parametri IN:
        camera          numero della telecamera
        destinazione    nome della seriale cui inviare i dati. Se NULL non invia i dati
        immON           se true mostra il cerchio trovato

    Questa versione riceve la richiesta dati (R) dalla stessa seriale su cui li inviaha
*/

int CercaUnCerchio(int camera, char *destinazione, bool immON)
{
    VideoCapture cap(camera); // open the video file for reading
    Mat frame,frame_grigio,frame_edges;
    char *sorgente;
    char stringa[25];
    int fd = 0;

    int baudrate = B9600;  // default

    sorgente=destinazione;
    cout << "Seriale sorgente " << sorgente << endl;
    if(sorgente!=NULL){
        fd = serialport_init(sorgente, baudrate);
        if(fd==-1) {
            cout << "Impossibile aprire la seriale per la ricezione comandi " << sorgente << endl;
            return -1;
        }
    }

    if(leggeParametriCalibrazione(nomeFileConfHough)<0)
    {
        cout << "Impossibile leggere i parametri di calibrazione! Impostato default" << endl;
    }

    if ( !cap.isOpened() )  // if not success, exit program
    {
        cout << "Impossibile aprire la cattuta video" << endl;
        return -1;
    }

    if(immON)namedWindow("Camera",CV_WINDOW_AUTOSIZE);

    while(1)
    {

        bool bSuccess = cap.read(frame); // read a new frame from video
        if (!bSuccess)
        {
            cout << "Impossibile leggere il frame" << endl;
            break;
        }

        cvtColor( frame, frame_grigio, CV_BGR2GRAY,0);

        /// Reduce the noise so we avoid false circle detection
        GaussianBlur( frame_grigio, frame_grigio, Size(nBlur, nBlur), 2, 2 );

        vector <Vec3f> circles;
        if(sorgente==NULL){ ///modalita` senza invio dati sulla seriale - Si usa per calibrare i parametri di ricerca

            /// Apply the Hough Transform to find the circles
            HoughCircles( frame_grigio, circles, CV_HOUGH_GRADIENT, accInvRatioResolution,minDistBetweenCenters, edgeThreshold, accThreshold, 0, 0 );

            /**
                Se ha trovato un cerchio valido
            */
            if(circles.size()!=0)
            {
                int xl,yl,rl;

                xl=cvRound(circles[0][0]);
                yl=cvRound(circles[0][1]);
                rl=cvRound(circles[0][2]);

                Point center(xl, yl);

                sprintf(stringa,"<%d-%d-%d>",xl,yl,rl);

                if(rl<=maxRaggio && rl>=minRaggio)  ///filtro sulla dimensione del cerchio trovato
                {

                    if(immON)
                    {
                        circle( frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
                        circle( frame, center, rl, Scalar(0,0,255), 2, 8, 0 );
                        putText(frame,stringa,center,FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 0), 2);
                    }
                }
            }
        }
        else{ //modalita` operativa

            if(riceviRichiesta(fd)==0)
            {
                    /// Apply the Hough Transform to find the circles
                HoughCircles( frame_grigio, circles, CV_HOUGH_GRADIENT, accInvRatioResolution,minDistBetweenCenters, edgeThreshold, accThreshold, 0, 0 );

                /**
                    Se ha trovato un cerchio valido
                */
                if(circles.size()!=0)
                {

                    x=cvRound(circles[0][0]);
                    y=cvRound(circles[0][1]);
                    r=cvRound(circles[0][2]);


                    if(r<=maxRaggio && r>=minRaggio)  ///filtro sulla dimensione del cerchio trovato
                    {
                        if(immON)
                        {
                            Point center(x, y);
                            circle( frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
                            circle( frame, center, r, Scalar(0,0,255), 2, 8, 0 );
                            putText(frame,stringa,center,FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 0), 2);
                        }
                        cout << "Richiesta ricevuta invio " ;
                        cout << "<"<< x <<"-"<< y <<"-"<< r <<">"<<endl;
                        if(invia3Interi(destinazione,x,y,r)!=0)
                            cout << "Cerchio non inviato" << endl;
                    }
                }
                else{
                    cout << "invio " ;
                        x=-1;y=-1;r=-1;
                        cout << "<"<< x <<"-"<< y <<"-"<< r <<">"<<endl;
                        if(invia3Interi(destinazione,x,y,r)!=0)
                            cout << "cerchio non valido" << endl;
                }
            }

        }

        if(immON)imshow("Camera", frame); //show the frc
        string s;
        int key;
        key=waitKey(40);
        if(key == 27)
        {
            break;
        }

    }
    if(immON)destroyWindow("Camera");
    cap.~VideoCapture();
    return 0;

}
