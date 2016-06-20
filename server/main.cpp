#include "stdafx.h"
#include "CepollServer.h"
#include "Cuser.h"






int main(void)
{
    auto a = CepollServer::GetInstance();
    a->InitServer(12138);
    a->Run();	
    return 0;
}
