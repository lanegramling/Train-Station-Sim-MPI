#ifndef TRAIN_H
#define TRAIN_H

//This class simply serves as a means to clarify management of Train routes.
//Can be extended to support functionality for the optional extras.
class Train
{
public:
    int* route;      //Catalogs the route of a train as an array of integers.
    int routeLength; //Stores a train's route length for convenience
    int destination; //Stores current destination, for convenience
    int currentStop; //The current stop along the route as an index of route[], for convenience.
    int timestep;    //Tracks timestep, for convenience
    char name;       //Character to label the train, for convenience
};

#endif
