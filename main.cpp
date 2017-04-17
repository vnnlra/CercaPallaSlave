#include <iostream>
#include <fstream>
#include "string.h"


#include "TrovaCerchio.h"

using namespace std;

void menuMain()
{
    cout <<  endl <<"--------------------------------------------------------------"<<endl;
    cout <<"Opzioni:"<<endl;
    cout <<"    1   ->  trova una pallina e invia centro e raggio a richiesta"<< endl;
    cout <<"    c   ->  aggiusta le soglie"<< endl;
    cout <<"    q   ->  esce"<<endl;
    cout <<"--------------------------------------------------------------"<<endl;
    cout <<"Fai scelta.."<<endl;
}

void menuUso()
{

    cout <<  endl <<"--------------------------------------------------------------"<<endl;
    cout <<"Uso:"<<endl;
    cout <<" ./CercaPallaSlave Ncam NomeSeriale"<< endl;
    cout <<"    Ncam          ->   numero telecamera"<<endl;
    cout <<"    NomeSeriale   ->   nome della seriale a cui inviare i dati"<<endl;
    cout <<"                       /dev/ttyACM0     arduino classico linux"<<endl;
    cout <<"                       /dev/ttyMCC      Neo seriale interna"<<endl;
    cout <<"                       /dev/ttymcx3      Neo seriale interna"<<endl;
    cout <<" ./CercaPallaSlave"<< endl;
    cout <<"    esegue in modalita` predefinita nel file CercaPalla.conf"<<endl;
    cout <<"--------------------------------------------------------------"<<endl;
}



int main(int argc, char* argv[])
{
    bool stop=false;
    char key;
    int camera=0;
    FILE *fconf;
    char stringa[20],seriale[20];
    char path[256],nomeFileConf[256],comando[256];


        strcpy(path,argv[0]);
        for(int i=strlen(path); i > 0; i--){
            if(path[i-1]=='/'){
                path[i]='\0';
                break;
            }
        }
    strcpy(nomeFileConfHough,path);
    strcat(nomeFileConfHough,"CalibrazioneHough.conf");

    strcpy(nomeFileConf,path);
    strcat(nomeFileConf,"CercaPalla.conf");

    printf("File conf esecuzione: %s\nFile conf algoritmo ricerca: %s\n",nomeFileConf,nomeFileConfHough);

    if(argc == 1){ //modalita` predefinita: run nella shell senza menu
                   //configurazione della seriale, camera e grafica in CercaPalla.conf

        fconf=fopen(nomeFileConf,"r");
        if(fconf!=NULL){
            fscanf(fconf,"%s %d",stringa,&camera);
            fscanf(fconf,"%s %s",stringa,seriale);
            printf("Camera: %d\nSeriale: %s\n",camera,seriale);
        }
        else{
            cout << "Impossibile aprire il file CercaPalla.conf" << endl;
            return -1;
        }
        if(CercaUnCerchio(camera, seriale,false)!=0){
            cout << "Errore nella ricerca cerchio"<<endl;
            return -1;
        }
    }
    else if(argc!=3)
    {
        menuUso();
        return -1;
    }
    else{
        camera=atoi(argv[1]);
        sprintf(seriale,"%s",argv[2]);
        printf("Camera: %d\nSeriale: %s\n",camera,seriale);
        do
        {
            menuMain();
            cin>>key;
            switch(key)
            {

            case '1':
                if(CercaUnCerchio(camera, seriale,true)!=0)
                {
                    cout << "Errore nella ricerca cerchio"<<endl;
                    return -1;
                }
                break;
            case 'c':

                /**Apre nano per editare il file  configurazione*/
                sprintf(comando,"nano %s",nomeFileConfHough);
                system(comando);

                //avvia la ricerca con la grafica e senza l'invio sulla seriale
                if(CercaUnCerchio(camera, NULL,true)!=0)
                {
                    cout << "Errore nella ricerca cerchio"<<endl;
                    return -1;
                }
                break;
            case 'q':
                stop=true;
                break;
            }
        }
        while(!stop);
    }

    return 0;

}
