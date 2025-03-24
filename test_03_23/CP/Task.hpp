#pragma once

class Task
{
public:
    Task() {};
    Task(int x, int y) : _x(x), _y(y)
    {}

    int X() {return _x;}
    int Y() {return _y;}   
    int Result(){return _result;}


    void Excute()
    {
        _result = _x + _y;
    }

private:
    int _x;
    int _y;
    int _result;

};