#ifndef DATATYPES_H
#define DATATYPES_H

//Job types
#define TransportJob 0
#define ChargingJob 1
#define ServiceJob 2
#define PauseJob 3

//Status
#define Available 0
#define Assigned 1
#define ReservedS 2
#define Inactive 3
#define Fault 4
#define ReadyForCharging 5
#define ReadyForReading 6
#define Charging 7
#define ConnectedS 8
#define Initial 9

struct Position
{
    double x;
    double y;
    double phi;
    double e;
};

struct Job
{
    int type;
    struct
    {
        int stationId;
        int placeId;
    } start;
    struct
    {
        int stationId;
        int placeId;
    } destination;
};

#endif // DATATYPES_H
