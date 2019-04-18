#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <exception>

#include "geos_wrap.h"

#define MSG_BUFFER_SIZE 256

namespace geos_wrap {

void 
geos_message_handler(const char *fmt, ...)
{
    char buffer[MSG_BUFFER_SIZE];
    va_list args;
    va_start (args, fmt);
    vsnprintf (buffer, MSG_BUFFER_SIZE, fmt, args);
    va_end (args);
    
    cerr << buffer << endl;
}

void 
geos_error_handler(const char *fmt, ...)
{
    char buffer[MSG_BUFFER_SIZE];
    va_list args;
    va_start (args, fmt);
    vsnprintf (buffer, MSG_BUFFER_SIZE, fmt, args);
    va_end (args);

    throw runtime_error(buffer);
}


GEOSCoordSequenceUniquePtr
make_CoordSeq(vector< pair<double, double> > & coords)
{
    auto seq = make_unique_w_free(GEOSCoordSeq_create(coords.size(), 2));
    for (unsigned int i = 0; i < coords.size(); i++) {
        int ret_x = GEOSCoordSeq_setX(seq.get(), i, coords[i].first);
        int ret_y = GEOSCoordSeq_setY(seq.get(), i, coords[i].second);

        if ((ret_x == 0) || (ret_y == 0)) {
            throw runtime_error("Could not set coordinate in sequence");
        }
    }
    return seq;
}

GEOSGeometryUniquePtr 
make_LinearRing(vector< pair<double, double> > & coords)
{
    auto seq = make_CoordSeq(coords);
    auto ring = make_unique_w_free(GEOSGeom_createLinearRing(seq.release()));
    if (ring.get() == nullptr) {
        throw runtime_error("Could not create linearring from coordinate sequence");
    }
    return ring;
}

GEOSGeometryUniquePtr 
make_Polygon(vector< vector< pair<double, double> > > & coords)
{
    vector<GEOSGeometryUniquePtr> ring_geometries;
    for (auto & cring : coords) {
        ring_geometries.push_back(make_LinearRing(cring));
    }

    if (ring_geometries.empty()) {
        auto polygon_geometry = make_unique_w_free(GEOSGeom_createEmptyPolygon());
        return polygon_geometry;
    }

    vector<GEOSGeometry *> holes;
    for (auto & ring_geometry : ring_geometries) {
        holes.push_back(ring_geometry.get());
    }

    auto polygon_geometry = make_unique_w_free(GEOSGeom_createPolygon(
        ring_geometries[0].get(),
        holes.data(),
        ring_geometries.size() - 1
    ));
    if (polygon_geometry.get() == nullptr) {
        throw std::runtime_error("Could not create polygon with all the rings");
    }

    // release ownership of the rings
    release_all(ring_geometries);

    return polygon_geometry;
}


GEOSGeometryUniquePtr 
make_Point(double x, double y)
{
    auto seq = make_unique_w_free(GEOSCoordSeq_create(1, 2));
    int ret_x = GEOSCoordSeq_setX(seq.get(), 0, x);
    int ret_y = GEOSCoordSeq_setY(seq.get(), 0, y);

    if ((ret_x == 0) || (ret_y == 0)) {
        throw runtime_error("Could not set coordinate in sequence");
    }
    auto point_geometry = make_unique_w_free(GEOSGeom_createPoint(seq.get()));
    if (point_geometry.get() == nullptr) {
        throw std::runtime_error("Could not create point");
    }
    seq.release(); // owned by the point now
    return point_geometry;
}

} // namespace geos_wrap
