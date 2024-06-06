#pragma once

#include "GUI/MemoryDiagnostics.h"
#include <cstdint>

struct DataPoint;

/* Custom allocator for DataPoints that's designed to make it easier
 * to see when a memory error occurred.
 */
namespace DataPointUtils {
    void* dataPointAlloc(size_t size,  bool isVector);
    void  dataPointFree (void* memory, bool isVector);
}

template <> struct MemoryDiagnostics::Allocator<DataPoint> {
    static void* scalarAlloc(std::size_t bytes) {
        return DataPointUtils::dataPointAlloc(bytes, false);
    }

    static void* vectorAlloc(std::size_t bytes) {
        return DataPointUtils::dataPointAlloc(bytes, true);
    }

    static void scalarFree(void* memory) {
        DataPointUtils::dataPointFree(memory, false);
    }

    static void vectorFree(void* memory) {
        DataPointUtils::dataPointFree(memory, true);
    }
};

/* Extra logic to try to make it easier to spot invalid reads or
 * writes to DataPoints.
 */
#define ADD_SAFETY_CHECKS_TO(Type)         \
    std::uint64_t _initializationFlag;     \
    DataPoint();                           \
    DataPoint(const std::string&, double); \
    ~DataPoint();                          \
    DataPoint(const DataPoint&);           \
    DataPoint(DataPoint&&);                \
    DataPoint& operator= (DataPoint)
