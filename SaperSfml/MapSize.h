#pragma once
#ifndef MapSize_H

#define MapSize_H

class NumOfCell {
public:
    short columns;
    short rows;
    NumOfCell() = default;
    NumOfCell(short c, short r);
    bool operator==(const NumOfCell& other) const;
};
#endif // !MapSize_H