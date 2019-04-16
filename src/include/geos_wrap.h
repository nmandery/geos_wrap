#ifndef __geos_wrap__
#define __geos_wrap__

#include <geos_c.h>

#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <utility>

using namespace std;

namespace geos_wrap {
    
void geos_message_handler(const char *fmt, ...);
void geos_error_handler(const char *fmt, ...);
    
inline void 
initialize_GEOS()
{
    initGEOS(&geos_message_handler, &geos_error_handler);
}


/**
 * Memory managment
 */
struct Free
{
    void operator()(GEOSGeometry * g) const {
        GEOSGeom_destroy(g);
    }

    void operator()(GEOSCoordSequence * cs) const {
        GEOSCoordSeq_destroy(cs);
    }

    void operator()(GEOSWKTWriter * w) const {
        GEOSWKTWriter_destroy(w);
    }

    void operator()(GEOSWKBWriter * w) const {
        GEOSWKBWriter_destroy(w);
    }

    void operator()(GEOSWKTReader * r) const {
        GEOSWKTReader_destroy(r);
    }

    void operator()(GEOSWKBReader * r) const {
        GEOSWKBReader_destroy(r);
    }

    void operator()(GEOSBufferParams * b) const {
        GEOSBufferParams_destroy(b);
    }

    void operator()(GEOSPreparedGeometry * g) const {
        GEOSPreparedGeom_destroy(g);
    }

    void operator()(GEOSSTRtree * t) const {
        GEOSSTRtree_destroy(t);
    }

    void operator()(char * str) const {
        GEOSFree(str);
    }

    void operator()(unsigned char * str) const {
        GEOSFree(str);
    }
};


template <typename T>
inline unique_ptr<T, Free> make_unique_w_free(T *t)
{
    return unique_ptr<T, Free>(t);
}

template <typename T>
inline shared_ptr<T> make_shared_w_free(T *t)
{
    return shared_ptr<T>(t, Free());
}

template< typename P >
inline void release_all( vector< P > & vec)
{
    for (auto & v : vec ) {
        v.release();
    }
}

/**
 * Typedefs
 */ 
typedef std::unique_ptr<GEOSGeometry, Free> GEOSGeometryUniquePtr;
typedef std::shared_ptr<GEOSGeometry> GEOSGeometrySharedPtr;
typedef std::unique_ptr<GEOSCoordSequence, Free> GEOSCoordSequenceUniquePtr;
typedef std::shared_ptr<GEOSCoordSequence> GEOSCoordSequenceSharedPtr;


/**
 * serialization
 */
inline string
GEOSGeometry_to_wkt_string(GEOSGeometry * g, int precision = 15)
{
    auto writer = make_unique_w_free(GEOSWKTWriter_create());
    GEOSWKTWriter_setRoundingPrecision(writer.get(), precision);
    auto wkt = make_unique_w_free(GEOSWKTWriter_write(writer.get(), g));
    return string(wkt.get());
}

inline string
GEOSGeometry_to_wkb_string(GEOSGeometry * g)
{
    auto writer = make_unique_w_free(GEOSWKBWriter_create());
    size_t num_chars;
    auto wkb = make_unique_w_free(GEOSWKBWriter_write(writer.get(), g, &num_chars));
    return string(reinterpret_cast<const char*>(wkb.get()), num_chars);
}

inline string
GEOSGeometry_to_hexwkb_string(GEOSGeometry * g)
{
    auto writer = make_unique_w_free(GEOSWKBWriter_create());
    size_t num_chars;
    auto wkb = make_unique_w_free(GEOSWKBWriter_writeHEX(writer.get(), g, &num_chars));
    return string(reinterpret_cast<const char*>(wkb.get()), num_chars);
}


/**
 * deserialization
 */
inline GEOSGeometryUniquePtr
wkt_string_to_GEOSGeometry(const string & s)
{
    auto reader = make_unique_w_free(GEOSWKTReader_create());
    auto g = make_unique_w_free(GEOSWKTReader_read(reader.get(), s.c_str()));
    return g;
}

inline GEOSGeometryUniquePtr
wkb_string_to_GEOSGeometry(const string & s)
{
    auto reader = make_unique_w_free(GEOSWKBReader_create());
    auto g = make_unique_w_free(GEOSWKBReader_read(
            reader.get(),
            reinterpret_cast<const unsigned char*>(s.c_str()),
            s.length()
    ));
    return g;
}

inline GEOSGeometryUniquePtr
hexwkb_string_to_GEOSGeometry(const string & s)
{
    auto reader = make_unique_w_free(GEOSWKBReader_create());
    auto g = make_unique_w_free(GEOSWKBReader_readHEX(
            reader.get(), 
            reinterpret_cast<const unsigned char*>(s.c_str()),
            s.length()
    ));
    return g;
}


/**
 * constructors
 */
GEOSCoordSequenceUniquePtr make_CoordSeq(vector< pair<double, double> > & coords);
GEOSGeometryUniquePtr make_LinearRing(vector< pair<double, double> > & coords);
GEOSGeometryUniquePtr make_Polygon(vector< vector< pair<double, double> > > & coords);

} // namespace geos_wrap

#endif // __geos_wrap__
