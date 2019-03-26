#include <fstream>   //File IO
#include <iostream>  //User IO
#include <string>    //String convenience
#include <thread>    //Threads
#include <chrono>    //For try_lock_for convenience

#include "Barrier.h"
#include "Train.h"

std::mutex* stations;       //Locks for each station

//Read an input file and create its array structure
void loadData(std::string fName, Train *&t, int& nTrains, int& nStations) {
    std::ifstream file(fName);
    int token; //used for grabbing tokens from data
    std::cout << "\n\nAttempting to read in from file " << fName << "...\n";

    if (file.is_open()) {
        file >> nTrains;    //Read nTrains
        file >> nStations;  //Read nStations

        t = new Train[nTrains];               //Intialize Trains
        for (int i = 0; file >> token; i++) {  //Initialize routes for each train
            t[i].routeLength = token;
            t[i].route = new int[t[i].routeLength];
            for (int j = 0; j < t[i].routeLength; j++) if (file >> token) t[i].route[j] = token;
        }
        file.close();
    }else{  //File failed to open
     std::cout << "\nError: File could not be opened. Please make sure you're using a valid file.";
     exit(0);
    }
}

//Threads (drivers) use this method to drive their train.
void drive(int trainNum, Train* trains, int nTrains, Barrier* b ) {
    int stop = 0;                                                                             //Start the day for the trains.
    Train* train = &trains[trainNum];
    train->name = static_cast<char>(65 + trainNum);                                            //Label trains for prettier user IO
    train->timestep = 0;
    do {
        train->currentStop = train->route[stop];
        train->destination = (train->route[stop + 1]) ? train->route[stop + 1] : train->route[stop];
        std::unique_lock<std::mutex> lockStation(stations[train->destination], std::defer_lock);
        if (train->currentStop != train->destination){
          if (lockStation.try_lock()) {
                  std::cout << "\nAt time step " << train->timestep << ": train " << train->name
                            << " is going from station " << train->currentStop << " to station " << train->destination;
              stop++;
              b->barrier(nTrains); //Synchronize each on timestep
              lockStation.unlock();
          } else {
              std::cout << "\nAt time step " << train->timestep << ": train " << train->name
                        << " must stay at station " << train->currentStop;
              b->barrier(nTrains);
          }
      }
        train->timestep++;
    } while (stop < train->routeLength);

}

//main - input file name defaults to data.txt if first argument is null.
int main(int argc, char* argv[]) {
    std::string fName = (argv[1]) ? argv[1] : "data.txt";
    Train* trains;
    int nTrains, nStations;

    loadData(fName, trains, nTrains, nStations);       //Load the file into our variables

    Barrier* b = new Barrier;                               //Barrier group
    std::thread** drivers = new std::thread*[nTrains];      //New threads
    stations = new std::mutex[nStations];                   //Initialize mutexes

    for (int train = 0; train < nTrains; train++) drivers[train] = new std::thread(drive, train, trains, nTrains, b);
    for (int i = 0; i < nTrains; i++) drivers[i]->join();

    //Summary
    std::cout << "\nSimulation Complete\n";
    for (int i = 0; i < nTrains; i++) std::cout << "\nTrain " << trains[i].name << " completed its route at time step " << trains[i].timestep;




    std::cout << "\n\n";
}
