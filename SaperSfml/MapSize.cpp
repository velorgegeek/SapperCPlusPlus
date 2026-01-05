#include "MapSize.h"


NumOfCell::NumOfCell(short c, short r) : columns(c), rows(r) {}

bool NumOfCell::operator==(const NumOfCell& other) const {
    return other.columns == columns && other.rows == rows;
}