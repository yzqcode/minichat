#ifndef CUSER_H
#define CUSER_H

#include "stdafx.h"
class Cuser
{
    public:
        Cuser();
        ~Cuser();
        string Getname() { return m_name; }
        void Setname(string val) { m_name = val; }
    protected:
    private:
        string m_name;
};

#endif // CUSER_H
