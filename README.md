# Server-Side-Google-Map-Markers-Clustering-C-plus-plus-V8
Render cluster points on google map generated on server side (using C++ module for V8 engine) - very useful when you have a lot of items to show on map. This library is meant to be used as a C++ module on a node.js project.

If you are looking for a PHP solution you can find it here -> http://rtsoftwaregroup.io/server-side-google-map-markers-clustering/

---------------------------------------

At some point a developer could face the situation in which has to display on the map thousands of markers and this of course isn’t a good move to implement the commonly used MarkerCluster (http://google-maps-utility-library-v3.googlecode.com/svn/trunk/markerclusterer/docs/reference.html), because you don’t want to process all those markers on the browser, also the time to load all the data to the browser would be huge.

So we had to deal with the clustering algorithm and implement it on server side, as we googled around there were poor information on this direction and no body seems to have share a decent solution so far.

The most efficient algorithm for our scenario was the  Distance Based Clustering (http://en.wikipedia.org/wiki/Cluster_analysis) but there were an issue, we had to convert miles into pixels, so in this way we can calculate the distance from each point on the map and group the markers together for the given zoom on the map.

Also we had to use some Mercator (http://en.wikipedia.org/wiki/Mercator_projection) magic math, to convert latitude and longitude to pixel Xand Y values. On the source code below you may notice the OFFSET constant, which is half of the earth circumference in pixels at zoom level 21 (Max zoom on google map)

On the code below you will find the usage of couple params, Array with coordinates, Distance between points to create the cluster, Zoom level, and lately the more than number to create the cluster (we don’t want to create clusters with 2 or 4 points).

But since the algorithm have to handle several loops for creating the clusters and since we had to make it faster possible we had to develop it on C++, which is really fast. We have used this module on a node.js API and we just include the pre-compiled module like how we do normally with all the node modules require() – for all folks which doesn’t know how to develop a V8 module for node, follow this link http://nodejs.org/api/addons.html

V8 addon can be used like - 

var myAddon = require("pathTo/cluster")

myAddon.cluster(ArrayCoordinates, 20, 10, 10)
