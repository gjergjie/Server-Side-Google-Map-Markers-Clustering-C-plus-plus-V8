#include <v8.h>
#include <node.h>
#include <math.h>
#include <vector>

#define    OFFSET    268435456
#define    RADIUS    85445659.4471

#define    PI    3.141592653589793238462

using namespace v8;
using std::vector;

double lonToX(double lon) {
    return round(OFFSET + RADIUS * lon * PI / 180);
}

double latToY(double lat) {
    return round(OFFSET - RADIUS * log( (1 + sin( lat * PI / 180) ) / (1 - sin( lat * PI / 180 ) ) ) / 2);
}

long pixelDistance(double lat1, double lon1, double lat2, double lon2, int zoom) {
    double x1 = lonToX(lon1);
    double y1 = latToY(lat1);

    double x2 = lonToX(lon2);
    double y2 = latToY(lat2);

    return (long)sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2)) >> (21 - zoom);
}

Handle<Array> newClasterPoint(double x1, double x2, double y1, double y2, float movePercent) {

    Handle<Array> newPoint = Array::New();

    double pixel = sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));

    float cosin = (x1 - x2) / pixel;
    float sinus = (y1 - y2) / pixel;
    double distanceMovePixel = pixel * movePercent;
    double newXMove = cosin * distanceMovePixel;
    double newYMove = sinus * distanceMovePixel;

    newPoint->Set(0, Number::New(x1 - newXMove));
    newPoint->Set(1, Number::New(y1 - newYMove));
    return newPoint;

}

Handle<Value> cluster_method(const Arguments& args) {

    HandleScope scope;

    Handle<Array> inputArray;

    vector<Handle<Object> > markers;

    unsigned int moreThan = args[3]->NumberValue();

    if(moreThan > 0) moreThan -= 1;

    if (args[0]->IsArray()) {
        inputArray = Handle<Array>::Cast(args[0]);

        for (unsigned int i = 0; i < inputArray->Length(); i++)
            markers.push_back(Handle<Object>::Cast(inputArray->Get(i)));

    }

    Handle<Array> clustered = Array::New();

    for (unsigned int i = 0; i < markers.size(); ) {

        unsigned int cluster = 0;
        Handle<Object> marker = Handle<Object>::Cast(markers.at(0));

        markers.erase(markers.begin());

        vector<int> cluterFindedIndex;

        Handle<Array> clusterPoint = Array::New();
        clusterPoint = Handle<Array>::Cast(marker->Get(String::New("location")));

        float movePercent = 0.5;

        for (unsigned int j = 0; j < markers.size(); j++) {

            Handle<Object> arg = Handle<Object>::Cast(markers.at(j));
            Handle<Object> currentLocation = Handle<Object>::Cast(marker->Get(String::New("location")));
            Handle<Object> clusterLocation = Handle<Object>::Cast(arg->Get(String::New("location")));

            long pixel = pixelDistance(

                    currentLocation->Get(1)->NumberValue(),
                    currentLocation->Get(0)->NumberValue(),
                    clusterLocation->Get(1)->NumberValue(),
                    clusterLocation->Get(0)->NumberValue(),
                    args[2]->NumberValue()
                );

            if (args[1]->NumberValue() > pixel) {

                cluster++;
                cluterFindedIndex.push_back(j);

                clusterPoint = newClasterPoint(
                                                clusterPoint->Get(0)->NumberValue(),
                                                clusterLocation->Get(0)->NumberValue(),
                                                clusterPoint->Get(1)->NumberValue(),
                                                clusterLocation->Get(1)->NumberValue(),
                                                movePercent);

                movePercent -= (movePercent * 0.03);

            }
        }

        if (cluster > moreThan) {

            for (unsigned int k = 0; k < cluterFindedIndex.size(); k++) {
                markers.erase(markers.begin() + (cluterFindedIndex.at(k) - k)); 
            }

            Local<Object> clusterData = Object::New();

            marker->Delete(String::New("id"));

            clusterData->Set(String::NewSymbol("count"), Number::New(cluster + 1));
            clusterData->Set(String::NewSymbol("coordinate"), clusterPoint);

            clustered->Set(clustered->Length(), clusterData);
        } else {
            clustered->Set(clustered->Length(), marker);
        }

    }

    return scope.Close(clustered);
}

void init(Handle<Object> exports) {
        exports->Set(String::NewSymbol("cluster"), FunctionTemplate::New(cluster_method)->GetFunction());
}

NODE_MODULE(cluster, init)